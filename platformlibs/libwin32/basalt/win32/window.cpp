#include <basalt/win32/window.h>

#include <basalt/win32/build_config.h>
#include <basalt/win32/key_map.h>
#include <basalt/win32/util.h>

#if BASALT_TRACE_WINDOWS_MESSAGES
#include <basalt/win32/debug.h>
#endif // BASALT_TRACE_WINDOWS_MESSAGES

#include <basalt/win32utilities/util.h>

#include <basalt/api/shared/config.h>
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

Window::~Window() {
  if (!DestroyWindow(mHandle)) {
    BASALT_LOG_ERROR("DestroyWindow failed: {}",
                     create_win32_error_message(GetLastError()));
  }

  if (!UnregisterClassW(CLASS_NAME, mModuleHandle)) {
    BASALT_LOG_ERROR("UnregisterClassW failed: {}",
                     create_win32_error_message(::GetLastError()));
  }
}

auto Window::handle() const noexcept -> HWND {
  return mHandle;
}

auto Window::input_manager() noexcept -> InputManager& {
  return mInputManager;
}

auto Window::client_area_size() const noexcept -> Size2Du16 {
  return mClientAreaSize;
}

auto Window::current_mode() const noexcept -> WindowMode {
  return mCurrentMode;
}

void Window::set_mode(const WindowMode windowMode) {
  if (mCurrentMode == windowMode) {
    return;
  }

  auto style {static_cast<DWORD>(GetWindowLongPtrW(mHandle, GWL_STYLE))};
  RECT rect {};
  UINT swpFlags {SWP_NOZORDER};

  switch (windowMode) {
  case WindowMode::Windowed:
    style |= mSavedWindowInfo.style;
    rect = mSavedWindowInfo.rect;
    swpFlags |= SWP_FRAMECHANGED;

    break;

  case WindowMode::Fullscreen: {
    mSavedWindowInfo.style =
      static_cast<DWORD>(GetWindowLongPtrW(mHandle, GWL_STYLE));
    GetWindowRect(mHandle, &mSavedWindowInfo.rect);

    MONITORINFO mi {};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(MonitorFromWindow(mHandle, MONITOR_DEFAULTTONEAREST), &mi);

    rect = mi.rcMonitor;

    style &= ~WS_OVERLAPPEDWINDOW;
    swpFlags |= SWP_FRAMECHANGED;

    break;
  }

  case WindowMode::FullscreenExclusive:
    // TODO: support FullscreenExclusive
    BASALT_ASSERT_MSG(false, "not implemented");
    break;
  }

  SetWindowLongPtrW(mHandle, GWL_STYLE, style);
  SetWindowPos(mHandle, HWND_TOP, rect.left, rect.top, rect.right - rect.left,
               rect.bottom - rect.top, swpFlags);

  mCurrentMode = windowMode;
}

// issue with our input handling
//   WM_MOUSEMOVE is handled before WM_SETCURSOR and put in our
//   input queue -> the rest of the app gets the ability to set the
//   cursor only in the next frame -> the new cursor is only set at the
//   next WM_MOUSEMOVE/WM_SETCURSOR
void Window::set_cursor(const MouseCursor cursor) noexcept {
  mCurrentCursor = cursor;
  SetCursor(mLoadedCursors[enum_cast(mCurrentCursor)]);
}

auto Window::create(const HMODULE moduleHandle, const int showCommand,
                    const Desc& desc) -> WindowPtr {
  const ATOM windowClass {register_class(moduleHandle)};
  if (!windowClass) {
    throw system_error {static_cast<int>(GetLastError()),
                        std::system_category(),
                        "Failed to register window class"s};
  }

  // WS_CLIPSIBLINGS is added automatically (tested on Windows 10)
  DWORD style {WS_OVERLAPPEDWINDOW};
  if (!desc.resizeable) {
    style &= ~(WS_MAXIMIZEBOX | WS_SIZEBOX);
  }

  // WS_EX_WINDOWEDGE is added automatically (tested on Windows 10)
  const DWORD styleEx {};

  const wstring windowTitle {create_wide_from_utf8(desc.title)};
  Size2Du16 clientAreaSize = desc.preferredClientAreaSize;

  const HWND handle {
    CreateWindowExW(styleEx, reinterpret_cast<LPCWSTR>(windowClass),
                    windowTitle.c_str(), style, CW_USEDEFAULT, 0, CW_USEDEFAULT,
                    0, nullptr, nullptr, moduleHandle, &clientAreaSize)};
  if (!handle) {
    BASALT_LOG_ERROR("failed to create window");

    return nullptr;
  }

  // can't use make_unique because of the private constructor
  auto* const window = new Window {moduleHandle, handle};

  // unique_ptr is actually a lie
  // see the comment at the WM_CLOSE message for details
  SetWindowLongPtrW(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));

  ShowWindow(handle, showCommand);

  // required to be after ShowWindow because otherwise the application icon
  // would not appear in the titlebar if the application launches with
  // fullscreen and switches to windowed
  // TODO: find a better workaround
  window->set_mode(desc.mode);

  return unique_ptr<Window> {window};
}

Window::Window(const HMODULE moduleHandle, const HWND handle)
  : mModuleHandle {moduleHandle}, mHandle {handle} {
  BASALT_ASSERT(mModuleHandle);
  BASALT_ASSERT(mHandle);

  auto loadCursor = [](const LPCWSTR name, const UINT flags) -> HCURSOR {
    return static_cast<HCURSOR>(
      ::LoadImageW(nullptr, name, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | flags));
  };

  std::get<enum_cast(MouseCursor::Arrow)>(mLoadedCursors) =
    loadCursor(MAKEINTRESOURCEW(OCR_NORMAL), LR_SHARED);
  std::get<enum_cast(MouseCursor::TextInput)>(mLoadedCursors) =
    loadCursor(MAKEINTRESOURCEW(OCR_IBEAM), LR_SHARED);
  std::get<enum_cast(MouseCursor::ResizeAll)>(mLoadedCursors) =
    loadCursor(MAKEINTRESOURCEW(OCR_SIZEALL), LR_SHARED);
  std::get<enum_cast(MouseCursor::ResizeNS)>(mLoadedCursors) =
    loadCursor(MAKEINTRESOURCEW(OCR_SIZENS), LR_SHARED);
  std::get<enum_cast(MouseCursor::ResizeEW)>(mLoadedCursors) =
    loadCursor(MAKEINTRESOURCEW(OCR_SIZEWE), LR_SHARED);
  std::get<enum_cast(MouseCursor::ResizeNESW)>(mLoadedCursors) =
    loadCursor(MAKEINTRESOURCEW(OCR_SIZENESW), LR_SHARED);
  std::get<enum_cast(MouseCursor::ResizeNWSE)>(mLoadedCursors) =
    loadCursor(MAKEINTRESOURCEW(OCR_SIZENWSE), LR_SHARED);
  std::get<enum_cast(MouseCursor::Hand)>(mLoadedCursors) =
    loadCursor(MAKEINTRESOURCEW(OCR_HAND), LR_SHARED);
  std::get<enum_cast(MouseCursor::NotAllowed)>(mLoadedCursors) =
    loadCursor(MAKEINTRESOURCEW(OCR_NO), LR_SHARED);
}

auto Window::handle_message(const UINT message, const WPARAM wParam,
                            const LPARAM lParam) -> LRESULT {
  switch (message) {
  case WM_SIZE:
    switch (wParam) {
    case SIZE_RESTORED:
    case SIZE_MAXIMIZED:
      mClientAreaSize = Size2Du16 {LOWORD(lParam), HIWORD(lParam)};
      break;

    default:
      break;
    }

    break;

  case WM_PAINT: {
    PAINTSTRUCT ps {};
    const HDC dc = ::BeginPaint(mHandle, &ps);
    if (dc && ps.fErase) {
      // this will never be called as long as our window class has a background
      // brush and ::DefWindowProcW handles WM_ERASEBKGND
      ::FillRect(dc, &ps.rcPaint, reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1));
    }

    EndPaint(mHandle, &ps);

    return 0;
  }

  case WM_CLOSE:
    // ::DefWindowProcW would destroy the window.
    // In order to not lie about the lifetime of the window object, we handle
    // the WM_CLOSE message here to trigger our normal shutdown path
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
    return 0;

  case WM_EXITSIZEMOVE: {
    update_client_area_size();

    return 0;
  }

  default:
    break;
  }

  return DefWindowProcW(mHandle, message, wParam, lParam);
}

void Window::process_mouse_message_states(const WPARAM wParam) {
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

void Window::update_client_area_size() {
  RECT rect {};
  GetClientRect(handle(), &rect);
  mClientAreaSize.set(static_cast<u16>(rect.right),
                      static_cast<u16>(rect.bottom));
}

auto Window::register_class(const HMODULE moduleHandle) -> ATOM {
  auto* const icon = static_cast<HICON>(
    LoadImageW(nullptr, MAKEINTRESOURCEW(OIC_SAMPLE), IMAGE_ICON, 0, 0,
               LR_DEFAULTSIZE | LR_SHARED));
  if (!icon) {
    BASALT_LOG_ERROR("failed to load icon");
  }

  const int smallIconSizeX = GetSystemMetrics(SM_CXSMICON);
  const int smallIconSizeY = GetSystemMetrics(SM_CYSMICON);
  auto* const smallIcon = static_cast<HICON>(
    LoadImageW(nullptr, MAKEINTRESOURCEW(OIC_SAMPLE), IMAGE_ICON,
               smallIconSizeX, smallIconSizeY, LR_SHARED));
  if (!smallIcon) {
    BASALT_LOG_ERROR("failed to load small icon");
  }

  WNDCLASSEXW windowClass {sizeof(WNDCLASSEXW),
                           CS_OWNDC,
                           &Window::window_proc,
                           0, // cbClsExtra
                           0, // cbWndExtra
                           moduleHandle,
                           icon,
                           nullptr,
                           reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1),
                           nullptr, // lpszMenuName
                           CLASS_NAME,
                           smallIcon};

  return RegisterClassExW(&windowClass);
}

auto CALLBACK Window::window_proc(const HWND handle, const UINT message,
                                  const WPARAM wParam, const LPARAM lParam)
  -> LRESULT {
#if BASALT_TRACE_WINDOWS_MESSAGES
  BASALT_LOG_TRACE("received message: {}",
                   message_to_string(message, wParam, lParam));
#endif // BASALT_TRACE_WINDOWS_MESSAGES

  if (const auto window = GetWindowLongPtrW(handle, GWLP_USERDATA)) {
    return reinterpret_cast<Window*>(window)->handle_message(message, wParam,
                                                             lParam);
  }

  switch (message) {
  case WM_CREATE: {
    const auto& cs = *reinterpret_cast<CREATESTRUCTW*>(lParam);
    const Size2Du16& clientAreaSize =
      *static_cast<Size2Du16*>(cs.lpCreateParams);

    MONITORINFO mi {};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(MonitorFromWindow(handle, MONITOR_DEFAULTTONEAREST), &mi);

    POINT topLeftClient {0, 0};
    ClientToScreen(handle, &topLeftClient);

    const Size2Du16 monitorSize {
      static_cast<u16>(mi.rcMonitor.right - mi.rcMonitor.left),
      static_cast<u16>(mi.rcMonitor.bottom - mi.rcMonitor.top)};

    const LONG ncOffsetX = topLeftClient.x - cs.x;
    const LONG ncOffsetY = topLeftClient.y - cs.y;

    const RECT rect =
      calc_window_rect(cs.x + ncOffsetX, cs.y + ncOffsetY, cs.style,
                       cs.dwExStyle, clientAreaSize, monitorSize, mi.rcWork);

    SetWindowPos(handle, HWND_TOP, rect.left, rect.top, rect.right - rect.left,
                 rect.bottom - rect.top, SWP_NOZORDER | SWP_NOACTIVATE);

    break;
  }
  default:
    break;
  }

  return DefWindowProcW(handle, message, wParam, lParam);
}

} // namespace basalt
