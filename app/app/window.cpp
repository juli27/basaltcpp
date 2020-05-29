#include "window.h"

#include "build_config.h"
#include "globals.h"
#include "key_map.h"
#include "util.h"

#if BASALT_TRACE_WINDOWS_MESSAGES
#include "debug.h"
#endif // BASALT_TRACE_WINDOWS_MESSAGES

#include "shared/util.h"

#include <runtime/Engine.h>

#include <runtime/platform/Platform.h>
#include <runtime/platform/events/Event.h>
#include <runtime/platform/events/KeyEvents.h>

#include <runtime/shared/Config.h>
#include <runtime/shared/Log.h>

#include <windowsx.h>

#include <algorithm>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

using namespace std::literals;

using std::runtime_error;
using std::string;
using std::system_error;
using std::unique_ptr;
using std::wstring;
using std::wstring_view;
using std::vector;

namespace basalt::win32 {

using gfx::backend::D3D9ContextFactory;
using gfx::backend::D3D9ContextFactoryPtr;
using gfx::backend::D3D9GfxContext;
using platform::Event;
using platform::Key;
using platform::KeyPressedEvent;
using platform::KeyReleasedEvent;
using platform::PlatformEventCallback;

vector<PlatformEventCallback> sEventListener;
WindowMode sWindowMode;

namespace {

void dispatch_platform_event(const Event&);

} // namespace

Window::~Window() {
  if (!::DestroyWindow(mHandle)) {
    BASALT_LOG_ERROR(
      "::DestroyWindow failed: {}"
    , create_winapi_error_message(::GetLastError()));
  }

  if (!::UnregisterClassW(CLASS_NAME, mModuleHandle)) {
    BASALT_LOG_ERROR(
      "::UnregisterClassW failed: {}"
    , create_winapi_error_message(::GetLastError()));
  }
}

auto Window::drain_input() -> Input {
  return std::move(mInput);
}

void Window::update(Engine& engine) {
  mCurrentCursor = engine.mouseCursor;
}

auto Window::create(
  const HMODULE moduleHandle, const int showCommand
, const Config& config) -> WindowPtr {
  const ATOM windowClass {register_class(moduleHandle)};
  if (!windowClass) {
    throw system_error {
      static_cast<int>(::GetLastError()), std::system_category()
    , "Failed to register window class"s
    };
  }

  // TODO: support other modes
  sWindowMode = WindowMode::Windowed;

  RECT rect {0l, 0l, config.windowSize.width(), config.windowSize.height()};
  // handle don't care cases
  if (rect.right == 0l) {
    rect.right = 1280l;
  }
  if (rect.bottom == 0l) {
    rect.bottom = 720l;
  }

  DWORD style {WS_OVERLAPPEDWINDOW};
  if (!config.isWindowResizeable) {
    style &= ~(WS_MAXIMIZEBOX | WS_SIZEBOX);
  }

  // WS_EX_WINDOWEDGE is added automatically (tested on W10)
  DWORD styleEx {WS_EX_WINDOWEDGE};

  // calculate the window size for the given client area size
  if (!::AdjustWindowRectEx(&rect, style, FALSE, styleEx)) {
    throw system_error {
      static_cast<int>(::GetLastError()), std::system_category()
    };
  }

  const auto windowWidth {static_cast<int>(rect.right - rect.left)};
  const auto windowHeight {static_cast<int>(rect.bottom - rect.top)};

  if (config.windowMode != WindowMode::Windowed) {
    BASALT_LOG_ERROR("fullscreen not implemented");
    /*style = WS_POPUP;
    styleEx |= WS_EX_TOPMOST;*/
  }

  const wstring windowTitle {create_wide_from_utf8(config.appName)};
  const HWND handle {
    ::CreateWindowExW(
      styleEx, reinterpret_cast<LPCWSTR>(windowClass), windowTitle.c_str()
    , style, CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight, nullptr
    , nullptr, moduleHandle, nullptr)
  };
  if (!handle) {
    throw runtime_error("failed to create window");
  }

  // TODO: error handling
  auto factory = D3D9ContextFactory::create().value();
  auto gfxContext = factory->create_context(handle);

  // can't use make_unique because of the private constructor
  auto* const window = new Window {
    moduleHandle, handle, std::move(factory), std::move(gfxContext)
  , config.windowSize
  };

  // unique_ptr is actually a lie
  // see the comment at the WM_CLOSE message for details
  ::SetWindowLongPtrW(
    handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));

  ::ShowWindow(handle, showCommand);

  return unique_ptr<Window> {window};
}

Window::Window(
  const HMODULE moduleHandle, const HWND handle
, D3D9ContextFactoryPtr factory, unique_ptr<D3D9GfxContext> context
, const Size2Du16 clientAreaSize
)
  : mModuleHandle {moduleHandle}
  , mHandle {handle}
  , mFactory {std::move(factory)}
  , mContext {std::move(context)}
  , mClientAreaSize {clientAreaSize} {
  BASALT_ASSERT(mModuleHandle);
  BASALT_ASSERT(mHandle);
  BASALT_ASSERT(mFactory);
  BASALT_ASSERT(mContext);

  auto loadCursor = [](const LPCWSTR name, const UINT flags) -> HCURSOR {
    return static_cast<HCURSOR>(::LoadImageW(
      nullptr, name, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | flags));
  };

  std::get<enum_cast(MouseCursor::Arrow)>(mLoadedCursors) = loadCursor(
    MAKEINTRESOURCEW(OCR_NORMAL), LR_SHARED);
  std::get<enum_cast(MouseCursor::TextInput)>(mLoadedCursors) = loadCursor(
    MAKEINTRESOURCEW(OCR_IBEAM), LR_SHARED);
  std::get<enum_cast(MouseCursor::ResizeAll)>(mLoadedCursors) = loadCursor(
    MAKEINTRESOURCEW(OCR_SIZEALL), LR_SHARED);
  std::get<enum_cast(MouseCursor::ResizeNS)>(mLoadedCursors) = loadCursor(
    MAKEINTRESOURCEW(OCR_SIZENS), LR_SHARED);
  std::get<enum_cast(MouseCursor::ResizeEW)>(mLoadedCursors) = loadCursor(
    MAKEINTRESOURCEW(OCR_SIZEWE), LR_SHARED);
  std::get<enum_cast(MouseCursor::ResizeNESW)>(mLoadedCursors) = loadCursor(
    MAKEINTRESOURCEW(OCR_SIZENESW), LR_SHARED);
  std::get<enum_cast(MouseCursor::ResizeNWSE)>(mLoadedCursors) = loadCursor(
    MAKEINTRESOURCEW(OCR_SIZENWSE), LR_SHARED);
  std::get<enum_cast(MouseCursor::Hand)>(mLoadedCursors) = loadCursor(
    MAKEINTRESOURCEW(OCR_HAND), LR_SHARED);
  std::get<enum_cast(MouseCursor::NotAllowed)>(mLoadedCursors) = loadCursor(
    MAKEINTRESOURCEW(OCR_NO), LR_SHARED);
}

auto Window::dispatch_message(
  const UINT message, const WPARAM wParam, const LPARAM lParam
) -> LRESULT {
  switch (message) {
  case WM_SIZE:
    switch (wParam) {
    case SIZE_RESTORED:
      if (!mInSizingMode) {
        if (const Size2Du16 newSize(LOWORD(lParam), HIWORD(lParam));
          mClientAreaSize != newSize) {
          mClientAreaSize = newSize;

          do_resize(mClientAreaSize);
        }
      }
      break;

    case SIZE_MAXIMIZED:
      if (const Size2Du16 newSize(LOWORD(lParam), HIWORD(lParam));
        mClientAreaSize != newSize) {
        mClientAreaSize = {LOWORD(lParam), HIWORD(lParam)};
        do_resize(mClientAreaSize);
      }
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

    ::EndPaint(mHandle, &ps);

    return 0;
  }

  case WM_CLOSE:
    // ::DefWindowProcW would destroy the window.
    // In order to not lie about the lifetime of the window object, we handle
    // the WM_CLOSE message here and post the quit message to trigger our normal
    // shutdown path
    ::PostQuitMessage(0);
    return 0;

  case WM_SETCURSOR:
    // TODO: issue with our input handling
    //       the WM_MOUSEMOVE before the set cursor is handled and put in our
    //       input queue -> the rest of the app gets the ability to set the
    //       cursor only in the next frame -> the new cursor is only set at the
    //       next WM_MOUSEMOVE/WM_SETCURSOR
    if (LOWORD(lParam) == HTCLIENT) {
      ::SetCursor(mLoadedCursors[enum_cast(mCurrentCursor)]);
      return TRUE;
    }

    break;

  case WM_KEYDOWN:
  case WM_KEYUP: {
    const auto vkCode = static_cast<u8>(wParam);
    auto keyCode = VK_TO_KEY_MAP[vkCode];
    if (vkCode == VK_RETURN && (HIWORD(lParam) & KF_EXTENDED)) {
      keyCode = Key::NumpadEnter;
    }

    if (message == WM_KEYDOWN) {
      // don't dispatch repeat events
      if (HIWORD(lParam) & KF_REPEAT) {
        return 0;
      }

      // HACK: AltGr sends Ctrl + right Alt keydown messages but only sends
      // a keyup message for right Alt
      if (keyCode == Key::Control) {
        const DWORD ctrlMessageTime = ::GetMessageTime();
        MSG next {};
        if (::PeekMessageW(&next, nullptr, 0u, 0u, PM_NOREMOVE)) {
          if (next.message == WM_KEYDOWN) {
            if (next.wParam == VK_MENU
              && (HIWORD(next.lParam) & KF_EXTENDED)
              && next.time == ctrlMessageTime) {
              // skip ctrl message
              return 0;
            }
          }
        }
      }

      dispatch_platform_event(KeyPressedEvent(keyCode));
    } else {
      dispatch_platform_event(KeyReleasedEvent(keyCode));
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

    for (u16 repCount {LOWORD(lParam)}; repCount > 0; repCount--) {
      mInput.characters_typed(typedChar);
    }

    return 0;
  }

  case WM_MOUSEMOVE:
    mInput.mouse_moved(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    return 0;

  case WM_LBUTTONDOWN:
    mInput.mouse_button_pressed(MouseButton::Left);
    ::SetCapture(mHandle);
    return 0;

  case WM_LBUTTONUP:
    mInput.mouse_button_released(MouseButton::Left);
    if (!::ReleaseCapture()) {
      BASALT_LOG_ERROR(
        "Releasing mouse capture in WM_LBUTTONUP failed: {}",
        create_winapi_error_message(::GetLastError())
      );
    }
    return 0;

  case WM_RBUTTONDOWN:
    mInput.mouse_button_pressed(MouseButton::Right);
    return 0;

  case WM_RBUTTONUP:
    mInput.mouse_button_released(MouseButton::Right);
    return 0;

  case WM_MBUTTONDOWN:
    mInput.mouse_button_pressed(MouseButton::Middle);
    return 0;

  case WM_MBUTTONUP:
    mInput.mouse_button_released(MouseButton::Middle);
    return 0;

    // TODO: XBUTTON4 and XBUTTON5

  case WM_MOUSEWHEEL: {
    const f32 offset {
      static_cast<f32>(GET_WHEEL_DELTA_WPARAM(wParam)) / static_cast<f32>(
        WHEEL_DELTA)
    };
    mInput.mouse_wheel(offset);
    return 0;
  }

  case WM_ENTERSIZEMOVE:
    mInSizingMode = true;
    return 0;

  case WM_EXITSIZEMOVE: {
    mInSizingMode = false;
    RECT clientRect {};
    ::GetClientRect(mHandle, &clientRect);
    mClientAreaSize.set(
      static_cast<u16>(clientRect.right)
    , static_cast<u16>(clientRect.bottom));
    do_resize(mClientAreaSize);
    return 0;
  }

  default:
    break;
  }

  return ::DefWindowProcW(mHandle, message, wParam, lParam);
}

void Window::do_resize(const Size2Du16 clientArea) const {
  mContext->resize(clientArea);
}

auto Window::register_class(const HMODULE moduleHandle) -> ATOM {
  auto* const icon = static_cast<HICON>(::LoadImageW(
    nullptr, MAKEINTRESOURCEW(OIC_SAMPLE), IMAGE_ICON, 0, 0
  , LR_DEFAULTSIZE | LR_SHARED));
  if (!icon) {
    BASALT_LOG_ERROR("failed to load icon");
  }

  const int smallIconSizeX = ::GetSystemMetrics(SM_CXSMICON);
  const int smallIconSizeY = ::GetSystemMetrics(SM_CYSMICON);
  auto* const smallIcon = static_cast<HICON>(::LoadImageW(
    nullptr, MAKEINTRESOURCEW(OIC_SAMPLE), IMAGE_ICON, smallIconSizeX
  , smallIconSizeY, LR_SHARED));
  if (!smallIcon) {
    BASALT_LOG_ERROR("failed to load small icon");
  }

  WNDCLASSEXW windowClass {
    sizeof(WNDCLASSEXW)
  , CS_CLASSDC
  , &Window::window_proc
  , 0 // cbClsExtra
  , 0 // cbWndExtra
  , moduleHandle
  , icon
  , nullptr
  , reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1)
  , nullptr // lpszMenuName
  , CLASS_NAME
  , smallIcon
  };

  return ::RegisterClassExW(&windowClass);
}

auto CALLBACK Window::window_proc(
  const HWND handle, const UINT message, const WPARAM wParam
, const LPARAM lParam
) -> LRESULT {
#if BASALT_TRACE_WINDOWS_MESSAGES
  BASALT_LOG_TRACE(
    "received message: {}", message_to_string(message, wParam, lParam));
#endif // BASALT_TRACE_WINDOWS_MESSAGES

  if (const auto window = ::GetWindowLongPtrW(handle, GWLP_USERDATA)) {
    return reinterpret_cast<Window*>(window)->dispatch_message(
      message, wParam, lParam);
  }

  return ::DefWindowProcW(handle, message, wParam, lParam);
}

namespace {

void dispatch_platform_event(const Event& event) {
  std::for_each(
    sEventListener.cbegin(), sEventListener.cend(),
    [&event](const PlatformEventCallback& callback) {
      callback(event);
    }
  );
}

} // namespace

} // namespace basalt::win32
