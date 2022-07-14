#include <basalt/win32/window.h>

#include <basalt/win32/build_config.h>
#include <basalt/win32/key_map.h>
#include <basalt/win32/util.h>

#if BASALT_TRACE_WINDOWS_MESSAGES
#include <basalt/win32/debug.h>
#endif // BASALT_TRACE_WINDOWS_MESSAGES

#include <basalt/win32/shared/utils.h>

#include <basalt/gfx/backend/d3d9/factory.h>

#include <basalt/gfx/backend/context.h>

#include <basalt/api/shared/log.h>

#include <basalt/api/base/utils.h>

#include <windowsx.h>

#include <algorithm>
#include <numeric>
#include <string_view>
#include <system_error>

using namespace std::literals;

using std::array;
using std::string;
using std::system_error;
using std::unique_ptr;
using std::wstring;
using std::wstring_view;

namespace basalt {

namespace {

// posX and posY: location of the upper left corner of the client area
// clientArea is the preferred size of the client area (width and/or height can
// be 0 if no preference)
// workArea in virtual-screen coords
auto calc_window_rect(const int posX, const int posY, const DWORD style,
                      const DWORD styleEx, const Size2Du16 clientArea,
                      const Size2Du16 monitorSize,
                      const RECT& workArea) noexcept -> RECT {
  // window dimensions in client coords
  RECT rect {0l, 0l, clientArea.width(), clientArea.height()};

  // the default size is two thirds of the current display mode
  if (rect.right == 0l) {
    rect.right = MulDiv(monitorSize.width(), 2, 3);
  }
  if (rect.bottom == 0l) {
    rect.bottom = MulDiv(monitorSize.height(), 2, 3);
  }

  OffsetRect(&rect, posX, posY);

  // calculate the window size for the given client area size
  AdjustWindowRectEx(&rect, style, FALSE, styleEx);

  if (rect.right > workArea.right) {
    OffsetRect(&rect, -std::min(rect.right - workArea.right, rect.left), 0);
  }

  if (rect.bottom > workArea.bottom) {
    OffsetRect(&rect, 0, -std::min(rect.bottom - workArea.bottom, rect.top));
  }

  if (rect.right > workArea.right) {
    rect.right += workArea.right - rect.right;
  }

  if (rect.bottom > workArea.bottom) {
    rect.bottom += workArea.bottom - rect.bottom;
  }

  return rect;
}

} // namespace

auto Window::create(const HMODULE moduleHandle, const CreateInfo& info,
                    const gfx::D3D9Factory& gfxFactory) -> WindowPtr {
  const ATOM windowClass {register_class(moduleHandle)};
  if (!windowClass) {
    throw system_error {static_cast<int>(GetLastError()),
                        std::system_category(),
                        "Failed to register window class"s};
  }

  // WS_CLIPSIBLINGS is added automatically (tested on Windows 10)
  DWORD style {WS_OVERLAPPEDWINDOW};
  if (!info.resizeable) {
    style &= ~(WS_MAXIMIZEBOX | WS_SIZEBOX);
  }

  // WS_EX_WINDOWEDGE is added automatically (tested on Windows 10)
  constexpr DWORD styleEx {};

  const wstring windowTitle {create_wide_from_utf8(info.title)};

  auto window {std::make_unique<Window>(moduleHandle, windowClass,
                                        info.preferredClientAreaSize,
                                        gfxFactory.adapters())};

  const HWND handle {
    CreateWindowExW(styleEx, reinterpret_cast<LPCWSTR>(windowClass),
                    windowTitle.c_str(), style, CW_USEDEFAULT, 0, CW_USEDEFAULT,
                    0, nullptr, nullptr, moduleHandle, window.get())};
  if (!handle) {
    BASALT_LOG_ERROR("failed to create window");

    return nullptr;
  }

  // this pointer gets removed when the OS window data is destroyed by the
  // Window objects destructor
  SetWindowLongPtrW(handle, GWLP_USERDATA,
                    reinterpret_cast<LONG_PTR>(window.get()));

  ShowWindow(handle, info.showCommand);

  // required to be after ShowWindow because otherwise the application icon
  // would not appear in the titlebar if the application launches with
  // fullscreen and switches to windowed
  // TODO: find a better workaround
  window->set_mode(info.mode);

  window->init_gfx_context(gfxFactory);

  return window;
}

Window::~Window() {
  shutdown_gfx_context();

  VERIFY_WIN32_BOOL(DestroyWindow(mHandle));
  VERIFY_WIN32_BOOL(UnregisterClassW(reinterpret_cast<const WCHAR*>(mClassAtom),
                                     mModuleHandle));
}

auto Window::handle() const noexcept -> HWND {
  return mHandle;
}

auto Window::gfx_context() const noexcept -> const gfx::ContextPtr& {
  return mGfxContext;
}

auto Window::input_manager() noexcept -> InputManager& {
  return mInputManager;
}

auto Window::client_area_size() const noexcept -> Size2Du16 {
  return mClientAreaSize;
}

auto Window::mode() const noexcept -> WindowMode {
  return mCurrentMode;
}

auto Window::set_mode(const WindowMode windowMode) -> void {
  if (mCurrentMode == windowMode) {
    return;
  }

  if (windowMode == WindowMode::FullscreenExclusive) {
    set_mode(WindowMode::Fullscreen);

    mCurrentMode = windowMode;

    const auto& adapterInfo {mAdapters[0]};

    gfx::Context::ResetDesc desc {};
    desc.exclusiveDisplayMode = adapterInfo.displayMode;
    desc.renderTargetFormat = adapterInfo.displayFormat;
    desc.exclusive = true;
    mGfxContext->reset(desc);

    return;
  }

  // exclusive ownership of the output monitor needs to be released before
  // window changes can be made
  if (mCurrentMode == WindowMode::FullscreenExclusive) {
    mGfxContext->reset(gfx::Context::ResetDesc {});

    // the d3d9 runtime leaves the window as topmost when exiting exclusive
    // fullscreen
    SetWindowPos(mHandle, HWND_NOTOPMOST, 0, 0, 0, 0,
                 SWP_NOSIZE | SWP_NOSIZE | SWP_NOACTIVATE);

    mCurrentMode = WindowMode::Fullscreen;

    set_mode(windowMode);

    return;
  }

  auto style {static_cast<DWORD>(GetWindowLongPtrW(mHandle, GWL_STYLE))};

  if (mCurrentMode == WindowMode::Windowed) {
    mSavedWindowInfo.style = style;
    GetWindowRect(mHandle, &mSavedWindowInfo.windowRect);
  }

  mCurrentMode = windowMode;

  BASALT_ASSERT(mCurrentMode != WindowMode::FullscreenExclusive,
                "fullscreen exclusive mode must be handled by the gfx context");

  RECT rect {};

  switch (mCurrentMode) {
  case WindowMode::Windowed:
    style = mSavedWindowInfo.style;
    rect = mSavedWindowInfo.windowRect;

    break;

  case WindowMode::Fullscreen:
  case WindowMode::FullscreenExclusive: {
    MONITORINFO mi {};
    mi.cbSize = sizeof(MONITORINFO);
    GetMonitorInfoW(
      MonitorFromRect(&mSavedWindowInfo.windowRect, MONITOR_DEFAULTTONEAREST),
      &mi);

    rect = mi.rcMonitor;
    style &= ~WS_OVERLAPPEDWINDOW;

    break;
  }
  }

  SetWindowLongPtrW(mHandle, GWL_STYLE, style);

  // SWP_NOCOPYBITS causes the window to flash white
  constexpr UINT swpFlags {SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED};
  SetWindowPos(mHandle, nullptr, rect.left, rect.top, rect.right - rect.left,
               rect.bottom - rect.top, swpFlags);
}

// issue with our input handling
//   WM_MOUSEMOVE is handled before WM_SETCURSOR and put in our
//   input queue -> the rest of the app gets the ability to set the
//   cursor only in the next frame -> the new cursor is only set at the
//   next WM_MOUSEMOVE/WM_SETCURSOR
auto Window::set_cursor(const MouseCursor cursor) noexcept -> void {
  mCurrentCursor = cursor;
  SetCursor(mLoadedCursors[enum_cast(mCurrentCursor)]);
}

Window::Window(const HMODULE moduleHandle, const ATOM classAtom,
               const Size2Du16 clientAreaSize, const gfx::AdapterList& adapters)
  : mModuleHandle {moduleHandle}
  , mClassAtom {classAtom}
  , mAdapters {adapters}
  , mClientAreaSize {clientAreaSize} {
  BASALT_ASSERT(mModuleHandle);

  auto loadCursor {[](const WORD id) -> HCURSOR {
    return load_system_cursor(id, 0, 0, LR_DEFAULTSIZE);
  }};

  std::get<enum_cast(MouseCursor::Arrow)>(mLoadedCursors) =
    loadCursor(OCR_NORMAL);
  std::get<enum_cast(MouseCursor::TextInput)>(mLoadedCursors) =
    loadCursor(OCR_IBEAM);
  std::get<enum_cast(MouseCursor::ResizeAll)>(mLoadedCursors) =
    loadCursor(OCR_SIZEALL);
  std::get<enum_cast(MouseCursor::ResizeNS)>(mLoadedCursors) =
    loadCursor(OCR_SIZENS);
  std::get<enum_cast(MouseCursor::ResizeEW)>(mLoadedCursors) =
    loadCursor(OCR_SIZEWE);
  std::get<enum_cast(MouseCursor::ResizeNESW)>(mLoadedCursors) =
    loadCursor(OCR_SIZENESW);
  std::get<enum_cast(MouseCursor::ResizeNWSE)>(mLoadedCursors) =
    loadCursor(OCR_SIZENWSE);
  std::get<enum_cast(MouseCursor::Hand)>(mLoadedCursors) = loadCursor(OCR_HAND);
  std::get<enum_cast(MouseCursor::NotAllowed)>(mLoadedCursors) =
    loadCursor(OCR_NO);
}

auto Window::init_gfx_context(const gfx::D3D9Factory& gfxFactory) -> void {
  const auto& adapterInfo {mAdapters[0]};

  const gfx::D3D9Factory::DeviceAndContextDesc contextDesc {
    adapterInfo.handle,         adapterInfo.displayMode,
    adapterInfo.displayFormat,  gfx::ImageFormat::D16,
    gfx::MultiSampleCount::One, mCurrentMode == WindowMode::FullscreenExclusive,
  };

  auto [device, context] {
    gfxFactory.create_device_and_context(mHandle, contextDesc),
  };

  mGfxContext = std::move(context);

  BASALT_LOG_INFO("Direct3D9 context created: adapter={}, driver={}",
                  adapterInfo.displayName, adapterInfo.driverInfo);
}

auto Window::shutdown_gfx_context() -> void {
  mGfxContext.reset();
}

auto Window::handle_message(const UINT message, const WPARAM wParam,
                            const LPARAM lParam) -> LRESULT {
  switch (message) {
  case WM_CREATE: {
    const auto& cs {*reinterpret_cast<CREATESTRUCTW*>(lParam)};
    on_create(cs);

    break;
  }

  case WM_SIZE:
    switch (wParam) {
    case SIZE_RESTORED:
    case SIZE_MAXIMIZED:
      if (!mIsInSizeMoveModalLoop) {
        on_resize(Size2Du16 {LOWORD(lParam), HIWORD(lParam)});
      }

      break;

    default:
      break;
    }

    break;

  case WM_CLOSE:
    // DefWindowProcW would destroy the window. This would invalidate the window
    // handle and therefore put this Window object in an invalid state. In order
    // to prevent this, we handle the WM_CLOSE message here to trigger our
    // regular shutdown path
    PostQuitMessage(0);

    return 0;

  case WM_SETCURSOR:
    if (LOWORD(lParam) == HTCLIENT) {
      SetCursor(mLoadedCursors[enum_cast(mCurrentCursor)]);

      return TRUE;
    }
    break;

  case WM_KEYDOWN:
  case WM_KEYUP: {
    auto keyCode = VK_TO_KEY_MAP[wParam];
    const auto highLParam = HIWORD(lParam);
    if (wParam == VK_RETURN && highLParam & KF_EXTENDED) {
      keyCode = Key::NumpadEnter;
    }

    if (!(highLParam & KF_UP)) {
      // ignore repeat messages
      if (highLParam & KF_REPEAT) {
        return 0;
      }

      // HACK: AltGr sends Ctrl + right Alt keydown messages but only sends
      // a keyup message for right Alt
      if (keyCode == Key::Control) {
        MSG next {};
        if (PeekMessageW(&next, nullptr, 0u, 0u, PM_NOREMOVE)) {
          if (next.message == WM_KEYDOWN) {
            if (next.wParam == VK_MENU && HIWORD(next.lParam) & KF_EXTENDED &&
                next.time == static_cast<DWORD>(GetMessageTime())) {
              // skip ctrl message
              return 0;
            }
          }
        }
      }

      mInputManager.key_down(keyCode);
    } else {
      mInputManager.key_up(keyCode);
    }

    return 0;
  }

  case WM_CHAR: {
    // TODO: filter control characters
    // TODO: handle supplementary plane characters
    //       two messages are posted. create_utf8_from_wide handles the
    //       surrogates individually as invalid characters
    BASALT_ASSERT(wParam < 0x10000);
    const auto c {static_cast<wchar_t>(wParam)};
    const string typedChar {create_utf8_from_wide(wstring_view {&c, 1})};
    BASALT_ASSERT(typedChar.size() <= 4);
    array<char, 4> character {};
    std::copy_n(typedChar.begin(), std::min(typedChar.size(), 4ull),
                character.begin());

    for (u16 repCount {LOWORD(lParam)}; repCount > 0; repCount--) {
      mInputManager.character_utf8(character);
    }

    return 0;
  }

    // we process all passed input data (wParam & lParam) for every message:
    // "When mouse messages are posted faster than a thread can process them,
    // the system discards all but the most recent mouse message"
    // https://docs.microsoft.com/en-us/windows/win32/inputdev/about-mouse-input#mouse-messages
  case WM_MOUSEMOVE:
    // insert the mouse moved last, because this is the most recent message
    process_mouse_message_states(wParam);
    mInputManager.mouse_moved(
      PointerPosition {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
    return 0;

  case WM_LBUTTONDOWN:
  case WM_LBUTTONUP:
  case WM_RBUTTONDOWN:
  case WM_RBUTTONUP:
  case WM_MBUTTONDOWN:
  case WM_MBUTTONUP:
  case WM_XBUTTONDOWN:
  case WM_XBUTTONUP: {
    mInputManager.mouse_moved(
      PointerPosition {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
    process_mouse_message_states(wParam);

    constexpr u16 anyButton =
      MK_LBUTTON | MK_RBUTTON | MK_MBUTTON | MK_XBUTTON1 | MK_XBUTTON2;
    if (wParam & anyButton) {
      SetCapture(mHandle);
    } else {
      // release capture if all buttons are up
      // TODO: assert on the return value
      ReleaseCapture();
    }

    // WM_XBUTTONDOWN and WM_XBUTTONUP requires us to return TRUE
    return message == WM_XBUTTONDOWN || message == WM_XBUTTONUP;
  }

  case WM_MOUSEWHEEL: {
    POINT cursorPos {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
    ScreenToClient(mHandle, &cursorPos);
    mInputManager.mouse_moved(PointerPosition {cursorPos.x, cursorPos.y});
    process_mouse_message_states(LOWORD(wParam));
    const f32 offset {static_cast<f32>(GET_WHEEL_DELTA_WPARAM(wParam)) /
                      static_cast<f32>(WHEEL_DELTA)};
    mInputManager.mouse_wheel(offset);

    return 0;
  }

  case WM_ENTERSIZEMOVE:
    mIsInSizeMoveModalLoop = true;

    return 0;

  case WM_EXITSIZEMOVE: {
    mIsInSizeMoveModalLoop = false;

    RECT rect {};
    GetClientRect(mHandle, &rect);
    on_resize(
      Size2Du16 {static_cast<u16>(rect.right), static_cast<u16>(rect.bottom)});

    return 0;
  }

  default:
    break;
  }

  return DefWindowProcW(mHandle, message, wParam, lParam);
}

auto Window::on_create(const CREATESTRUCTW& cs) const -> void {
  MONITORINFO mi {};
  mi.cbSize = sizeof(MONITORINFO);
  GetMonitorInfoW(MonitorFromWindow(mHandle, MONITOR_DEFAULTTONEAREST), &mi);

  POINT topLeftClient {0, 0};
  ClientToScreen(mHandle, &topLeftClient);

  const Size2Du16 monitorSize {
    static_cast<u16>(mi.rcMonitor.right - mi.rcMonitor.left),
    static_cast<u16>(mi.rcMonitor.bottom - mi.rcMonitor.top)};

  const LONG ncOffsetX {topLeftClient.x - cs.x};
  const LONG ncOffsetY {topLeftClient.y - cs.y};

  const RECT rect {calc_window_rect(cs.x + ncOffsetX, cs.y + ncOffsetY,
                                    cs.style, cs.dwExStyle, mClientAreaSize,
                                    monitorSize, mi.rcWork)};

  SetWindowPos(mHandle, nullptr, rect.left, rect.top, rect.right - rect.left,
               rect.bottom - rect.top, SWP_NOZORDER | SWP_NOACTIVATE);
}

auto Window::on_resize(const Size2Du16 newClientAreaSize) -> void {
  if (mClientAreaSize == newClientAreaSize || mIsInSizeMoveModalLoop) {
    return;
  }

  mClientAreaSize = newClientAreaSize;

  // mGfxContext is null when this method is called from on_create through
  // SetWindowPos
  if (mGfxContext) {
    if (mCurrentMode != WindowMode::FullscreenExclusive &&
        mClientAreaSize != mGfxContext->surface_size()) {
      mGfxContext->reset(gfx::Context::ResetDesc {});
    }
  }
}

auto Window::process_mouse_message_states(const WPARAM wParam) -> void {
  if (wParam & MK_SHIFT) {
    mInputManager.key_down(Key::Shift);
  } else {
    mInputManager.key_up(Key::Shift);
  }
  if (wParam & MK_CONTROL) {
    mInputManager.key_down(Key::Control);
  } else {
    mInputManager.key_up(Key::Control);
  }
  if (wParam & MK_LBUTTON) {
    mInputManager.mouse_button_down(MouseButton::Left);
  } else {
    mInputManager.mouse_button_up(MouseButton::Left);
  }
  if (wParam & MK_RBUTTON) {
    mInputManager.mouse_button_down(MouseButton::Right);
  } else {
    mInputManager.mouse_button_up(MouseButton::Right);
  }
  if (wParam & MK_MBUTTON) {
    mInputManager.mouse_button_down(MouseButton::Middle);
  } else {
    mInputManager.mouse_button_up(MouseButton::Middle);
  }
  if (wParam & MK_XBUTTON1) {
    mInputManager.mouse_button_down(MouseButton::Button4);
  } else {
    mInputManager.mouse_button_up(MouseButton::Button4);
  }
  if (wParam & MK_XBUTTON2) {
    mInputManager.mouse_button_down(MouseButton::Button5);
  } else {
    mInputManager.mouse_button_up(MouseButton::Button5);
  }
}

auto Window::register_class(const HMODULE moduleHandle) -> ATOM {
  constexpr const WCHAR* className {L"BasaltWindow"};

  const WNDCLASSEXW windowClass {
    sizeof(WNDCLASSEXW),
    0, // style
    &Window::window_proc,
    0, // cbClsExtra
    0, // cbWndExtra
    moduleHandle,
    nullptr, // hIcon
    nullptr, // hCursor
    GetSysColorBrush(COLOR_WINDOW), // TODO: is the background brush needed?
    nullptr, // lpszMenuName
    className,
    nullptr, // hIconSm
  };

  return RegisterClassExW(&windowClass);
}

auto CALLBACK Window::window_proc(const HWND handle, const UINT message,
                                  const WPARAM wParam, const LPARAM lParam)
  -> LRESULT {
#if BASALT_TRACE_WINDOWS_MESSAGES
  BASALT_LOG_TRACE("received message: {}",
                   message_to_string(message, wParam, lParam));
#endif // BASALT_TRACE_WINDOWS_MESSAGES

  if (message == WM_CREATE) {
    const auto& cs {*reinterpret_cast<CREATESTRUCTW*>(lParam)};
    auto* const window {static_cast<Window*>(cs.lpCreateParams)};
    window->mHandle = handle;

    SetWindowLongPtrW(handle, GWLP_USERDATA,
                      reinterpret_cast<LONG_PTR>(window));
  }

  if (const auto window = GetWindowLongPtrW(handle, GWLP_USERDATA)) {
    return reinterpret_cast<Window*>(window)->handle_message(message, wParam,
                                                             lParam);
  }

  return DefWindowProcW(handle, message, wParam, lParam);
}

} // namespace basalt
