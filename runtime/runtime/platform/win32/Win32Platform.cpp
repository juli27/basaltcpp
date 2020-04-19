#include "D3D9GfxContext.h"
#include "Win32KeyMap.h"
#include "Win32Util.h"

#include "runtime/platform/Platform.h"

#include "runtime/platform/events/Event.h"
#include "runtime/platform/events/KeyEvents.h"
#include "runtime/platform/events/MouseEvents.h"
#include "runtime/platform/events/WindowEvents.h"

#include "runtime/shared/Asserts.h"
#include "runtime/shared/Size2D.h"
#include "runtime/shared/Types.h"

#include "runtime/shared/win32/Win32APIHeader.h"
#include "runtime/shared/win32/Win32SharedUtil.h"

// after windows.h
#include "runtime/shared/Log.h"

#include <windowsx.h> // GET_X_LPARAM, GET_Y_LPARAM

#include <algorithm> // for_each
#include <memory> // make_shared
#include <stdexcept>
#include <string>
#include <system_error> // system_category
#include <utility> // move
#include <vector>

namespace basalt::platform {

using ::std::runtime_error;
using ::std::shared_ptr;
using ::std::string;
using ::std::string_view;
using ::std::system_error;
using ::std::vector;
using ::std::wstring;
using ::std::wstring_view;

HINSTANCE sInstance;
int sShowCommand;

namespace {

struct WindowData final {
  WindowData() noexcept = default;
  WindowData(const WindowData&) noexcept = default;
  WindowData(WindowData&&) noexcept = default;
  ~WindowData() noexcept = default;

  auto operator=(const WindowData&) noexcept -> WindowData& = default;
  auto operator=(WindowData&&) noexcept -> WindowData& = default;

  HWND mHandle = nullptr;
  IGfxContext* mGfxContext = nullptr;
  //i32 mClientAreaWidth = 0;
  //i32 mClientAreaHeight = 0;
  Size2Du16 mClientAreaSize = Size2Du16::dont_care();
  WindowMode mMode = WindowMode::Windowed;
  bool mIsResizeable = false;
  bool mIsMinimized = false;
  bool mIsSizing = false;
};


constexpr wstring_view WINDOW_CLASS_NAME = L"BS_WINDOW_CLASS";

vector<PlatformEventCallback> sEventListener;
vector<shared_ptr<Event>> sPendingEvents;
WindowData sWindowData;

void register_window_class();
void create_main_window(const Config& config);
void dispatch_platform_event(const Event& event);
auto CALLBACK window_proc(
  HWND window, UINT message, WPARAM wParam, LPARAM lParam
) -> LRESULT;

} // namespace

void startup(const Config& config) {
  BASALT_ASSERT(sInstance, "Windows API not initialized");

  create_main_window(config);
}

void shutdown() {
  if (sWindowData.mHandle) {
    ::DestroyWindow(sWindowData.mHandle);
    sWindowData.mHandle = nullptr;
  }

  if (!::UnregisterClassW(
    WINDOW_CLASS_NAME.data(), sInstance
  )) {
    BASALT_LOG_ERROR(
      "failed to unregister window class: {}",
      create_winapi_error_message(::GetLastError())
    );
  }
}

void add_event_listener(const PlatformEventCallback& callback) {
  sEventListener.push_back(callback);
}

auto poll_events() -> vector<shared_ptr<Event>> {
  MSG msg{};
  while (::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
    ::TranslateMessage(&msg);
    ::DispatchMessageW(&msg);

    if (!msg.hwnd) {
      switch (msg.message) {
      case WM_QUIT:
        sPendingEvents.push_back(std::make_shared<QuitEvent>());
        break;

      default:
        // ‭275‬ is WM_TIMER
        // is received upon focus change
        BASALT_LOG_DEBUG("unhandled thread message: {}", msg.message);
        break;
      }
    }
  }

  return std::move(sPendingEvents);
}

auto wait_for_events() -> vector<shared_ptr<Event>> {
  MSG msg{};
  const auto ret = ::GetMessageW(&msg, nullptr, 0u, 0u);
  if (ret == -1) {
    BASALT_LOG_ERROR(create_winapi_error_message(::GetLastError()));
    // TODO: fixme
    BASALT_ASSERT(false, "GetMessage error");
  }

  // GetMessage retrieved WM_QUIT
  if (ret == 0) {
    sPendingEvents.push_back(std::make_shared<QuitEvent>());
    return sPendingEvents;
  }

  ::TranslateMessage(&msg);
  ::DispatchMessageW(&msg);

  // handle any remainig messages in the queue
  return poll_events();
}

auto get_name() -> std::string_view {
  return "Win32";
}

auto get_window_size() -> Size2Du16 {
  return sWindowData.mClientAreaSize;
}

auto get_window_mode() -> WindowMode {
  return sWindowData.mMode;
}

void set_window_mode(const WindowMode windowMode) {
  switch (windowMode) {
  case WindowMode::Windowed:
    break;
  case WindowMode::Fullscreen:
    break;
  case WindowMode::FullscreenExclusive:
    break;
  }
  BASALT_LOG_ERROR("platform::set_window_mode not implemented");
}

auto get_window_gfx_context() -> IGfxContext* {
  BASALT_ASSERT(sWindowData.mGfxContext, "no gfx context present");

  return sWindowData.mGfxContext;
}

namespace {

void register_window_class() {
  const auto cursor = static_cast<HCURSOR>(::LoadImageW(
    nullptr, MAKEINTRESOURCEW(OCR_NORMAL), IMAGE_CURSOR, 0, 0,
    LR_DEFAULTSIZE | LR_SHARED
  ));
  if (!cursor) {
    BASALT_LOG_ERROR("failed to load cursor");
  }

  WNDCLASSEXW windowClass{
    sizeof(WNDCLASSEXW),
    CS_OWNDC | CS_HREDRAW | CS_VREDRAW,
    &window_proc,
    0, // cbClsExtra
    0, // cbWndExtra
    sInstance,
    nullptr, // hIcon
    cursor,
    nullptr, // hbrBackground
    nullptr, // lpszMenuName
    WINDOW_CLASS_NAME.data(),
    nullptr // hIconSm
  };

  if (!::RegisterClassExW(&windowClass)) {
    throw system_error(
      ::GetLastError(), std::system_category(),
      "Failed to register window class"
    );
  }
}

void create_main_window(const Config& config) {
  register_window_class();

  sWindowData.mClientAreaSize = config.windowSize;
  sWindowData.mMode = config.windowMode;
  sWindowData.mIsResizeable = config.isWindowResizeable;

  // handle don't care cases
  if (sWindowData.mClientAreaSize.width() == 0) {
    sWindowData.mClientAreaSize.set_width(1280);
  }
  if (sWindowData.mClientAreaSize.height() == 0) {
    sWindowData.mClientAreaSize.set_height(720);
  }

  DWORD style = 0u;
  DWORD styleEx = WS_EX_APPWINDOW;
  if (config.windowMode == WindowMode::Windowed) {
    style |= WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

    if (config.isWindowResizeable) {
      style |= WS_MAXIMIZEBOX | WS_SIZEBOX;
    }
  } else {
    style |= WS_POPUP;
    styleEx |= WS_EX_TOPMOST;
  }

  auto windowWidth = ::GetSystemMetrics(SM_CXSCREEN);
  auto windowHeight = ::GetSystemMetrics(SM_CYSCREEN);

  if (config.windowMode == WindowMode::Windowed) {
    // calculate the window size for the given client area size
    // and center the window on the primary monitor
    RECT rect{0, 0, sWindowData.mClientAreaSize.width(), sWindowData.mClientAreaSize.height()};
    if (!::AdjustWindowRectEx(&rect, style, FALSE, styleEx)) {
      throw system_error(::GetLastError(), std::system_category());
    }

    windowWidth = static_cast<int>(rect.right - rect.left);
    windowHeight = static_cast<int>(rect.bottom - rect.top);
  }

  const auto windowTitle = create_wide_from_utf8(config.appName);
  sWindowData.mHandle = ::CreateWindowExW(
    styleEx, WINDOW_CLASS_NAME.data(), windowTitle.c_str(), style,
    CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight, nullptr, nullptr,
    sInstance, nullptr
  );
  if (!sWindowData.mHandle) {
    throw runtime_error("failed to create window");
  }

  ::ShowWindow(sWindowData.mHandle, sShowCommand);

  sWindowData.mGfxContext = new D3D9GfxContext(sWindowData.mHandle);
}

void dispatch_platform_event(const Event& event) {
  std::for_each(
    sEventListener.cbegin(), sEventListener.cend(),
    [&event](const PlatformEventCallback& callback) {
      callback(event);
    }
  );
}

auto CALLBACK window_proc(
  HWND window, const UINT message, const WPARAM wParam, const LPARAM lParam
) -> LRESULT {
  switch (message) {
  case WM_MOUSEMOVE:
    dispatch_platform_event(
      MouseMovedEvent({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)})
    );
    return 0;

  case WM_MOUSEWHEEL: {
    const auto offset = GET_WHEEL_DELTA_WPARAM(wParam) / static_cast<f32>(
      WHEEL_DELTA);
    dispatch_platform_event(MouseWheelScrolledEvent(offset));
    return 0;
  }

  case WM_LBUTTONDOWN:
    ::SetCapture(window);
    dispatch_platform_event(MouseButtonPressedEvent(MouseButton::Left));
    return 0;

  case WM_LBUTTONUP:
    if (!::ReleaseCapture()) {
      BASALT_LOG_ERROR(
        "Releasing mouse capture in WM_LBUTTONUP failed: {}",
        create_winapi_error_message(::GetLastError())
      );
    }
    dispatch_platform_event(MouseButtonReleasedEvent(MouseButton::Left));
    return 0;

  case WM_RBUTTONDOWN:
    dispatch_platform_event(MouseButtonPressedEvent(MouseButton::Right));
    return 0;

  case WM_RBUTTONUP:
    dispatch_platform_event(MouseButtonReleasedEvent(MouseButton::Right));
    return 0;

  case WM_MBUTTONDOWN:
    dispatch_platform_event(MouseButtonPressedEvent(MouseButton::Middle));
    return 0;

  case WM_MBUTTONUP:
    dispatch_platform_event(MouseButtonReleasedEvent(MouseButton::Middle));
    return 0;

    // TODO: XBUTTON4 and XBUTTON5

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
        MSG next{};
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
    const auto typedChar = create_utf8_from_wide(
      wstring(1, static_cast<WCHAR>(wParam))
    );

    string typedChars;
    auto repCount = LOWORD(lParam);
    for (; repCount > 0; repCount--) {
      typedChars.append(typedChar);
    }

    dispatch_platform_event(CharactersTyped(typedChars));
    return 0;
  }

  case WM_SETFOCUS:
    return 0;

  case WM_KILLFOCUS:
    if (sWindowData.mMode != WindowMode::Windowed) {
      ::ShowWindow(sWindowData.mHandle, SW_MINIMIZE);
    }
    return 0;

  case WM_ENTERSIZEMOVE:
    sWindowData.mIsSizing = true;
    return 0;

  case WM_EXITSIZEMOVE:
    sWindowData.mIsSizing = false;
    sPendingEvents.push_back(
      std::make_shared<WindowResizedEvent>(sWindowData.mClientAreaSize)
    );
    return 0;

  case WM_SIZE:
    switch (wParam) {
    case SIZE_MINIMIZED:
      sWindowData.mIsMinimized = true;
      sPendingEvents.push_back(std::make_shared<WindowMinimizedEvent>());
      return 0;
    case SIZE_RESTORED:
      if (sWindowData.mIsMinimized) {
        sWindowData.mIsMinimized = false;
        sPendingEvents.push_back(std::make_shared<WindowRestoredEvent>());
      }
      if (!sWindowData.mIsSizing) {
        if (const Size2Du16 newSize(LOWORD(lParam), HIWORD(lParam)); sWindowData.mClientAreaSize != newSize) {
          sWindowData.mClientAreaSize = newSize;

          sPendingEvents.push_back(std::make_shared<WindowResizedEvent>(
            sWindowData.mClientAreaSize));
        }
      }

      return 0;
    case SIZE_MAXIMIZED:
      sWindowData.mClientAreaSize = {LOWORD(lParam), HIWORD(lParam)};
      sPendingEvents.push_back(
        std::make_shared<WindowResizedEvent>(sWindowData.mClientAreaSize)
      );
      return 0;

    default:
      return 0;
    }

  case WM_CLOSE:
    sPendingEvents.push_back(std::make_shared<WindowCloseRequestEvent>());
    return 0;

  case WM_DESTROY:
    delete sWindowData.mGfxContext;
    sWindowData.mGfxContext = nullptr;
    sWindowData.mHandle = nullptr;
    ::PostQuitMessage(0);
    return 0;

  default:
    return ::DefWindowProcW(window, message, wParam, lParam);
  }
}

} // namespace

} // namespace basalt::platform