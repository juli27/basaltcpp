#include "runtime/platform/win32/key_map.h"
#include "runtime/platform/win32/messages.h"
#include "runtime/platform/win32/util.h"

#include "runtime/gfx/backend/d3d9/context_factory.h"

#include "runtime/platform/Platform.h"

#include "runtime/platform/events/Event.h"
#include "runtime/platform/events/KeyEvents.h"
#include "runtime/platform/events/MouseEvents.h"
#include "runtime/platform/events/WindowEvents.h"

#include "runtime/shared/Asserts.h"
#include "runtime/shared/Size2D.h"
#include "runtime/shared/Types.h"

#include "runtime/shared/win32/util.h"
#include "runtime/shared/win32/Windows_custom.h"

// after windows.h
#include "runtime/shared/Log.h"

#include <windowsx.h> // GET_X_LPARAM, GET_Y_LPARAM

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

namespace basalt::platform {

using ::std::runtime_error;
using ::std::shared_ptr;
using ::std::string;
using ::std::string_view;
using ::std::system_error;
using ::std::unique_ptr;
using ::std::vector;
using ::std::wstring;

HINSTANCE sInstance;
int sShowCommand;

namespace {

struct WindowData final {
  WindowData() noexcept = default;
  WindowData(const WindowData&) noexcept = delete;
  WindowData(WindowData&&) noexcept = default;
  ~WindowData() noexcept = default;

  auto operator=(const WindowData&) noexcept -> WindowData& = delete;
  auto operator=(WindowData&&) noexcept -> WindowData& = default;

  HWND handle {nullptr};
  unique_ptr<D3D9ContextFactory> factory {};
  unique_ptr<IGfxContext> gfxContext {};
  Size2Du16 clientAreaSize {Size2Du16::dont_care()};
  WindowMode mode {WindowMode::Windowed};
  bool isResizeable {false};
  bool isMinimized {false};
  bool isSizing {false};
};


constexpr auto WINDOW_CLASS_NAME = L"BS_WINDOW_CLASS";

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
  BASALT_ASSERT_MSG(sInstance, "win32 globals not initialized");

  create_main_window(config);
}

void shutdown() {
  if (sWindowData.handle) {
    ::DestroyWindow(sWindowData.handle);
    sWindowData.handle = nullptr;
  }

  if (!::UnregisterClassW(WINDOW_CLASS_NAME, sInstance)) {
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
    BASALT_ASSERT_MSG(false, "::GetMessageW error");
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
  return sWindowData.clientAreaSize;
}

auto get_window_mode() -> WindowMode {
  return sWindowData.mode;
}

void set_window_mode(const WindowMode windowMode) {
  switch (windowMode) {
  case WindowMode::Windowed:
    BASALT_LOG_ERROR(
      "platform::set_window_mode for Windowed not implemented");
    break;
  case WindowMode::Fullscreen:
    BASALT_LOG_ERROR(
      "platform::set_window_mode for Fullscreen not implemented");
    break;
  case WindowMode::FullscreenExclusive:
    BASALT_LOG_ERROR(
      "platform::set_window_mode for FullscreenExclusive not implemented");
    break;
  }
}

auto get_window_gfx_context() -> IGfxContext* {
  BASALT_ASSERT(sWindowData.gfxContext);

  return sWindowData.gfxContext.get();
}

namespace {

void register_window_class() {
  auto* const cursor = static_cast<HCURSOR>(::LoadImageW(
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
    reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1),
    nullptr, // lpszMenuName
    WINDOW_CLASS_NAME,
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

  sWindowData.mode = config.windowMode;
  sWindowData.isResizeable = config.isWindowResizeable;

  RECT rect {0, 0, config.windowSize.width(), config.windowSize.height()};
  // handle don't care cases
  if (rect.right == 0) {
    rect.right = 1280;
  }
  if (rect.bottom == 0) {
    rect.bottom = 720;
  }

  DWORD style = WS_OVERLAPPEDWINDOW;
  if (!config.isWindowResizeable) {
    style &= ~(WS_MAXIMIZEBOX | WS_SIZEBOX);
  }

  DWORD styleEx = 0u;

  // calculate the window size for the given client area size
  if (!::AdjustWindowRectEx(
    &rect, style, FALSE, styleEx)) {
    throw system_error(::GetLastError(), std::system_category());
  }

  const auto windowWidth = static_cast<int>(rect.right - rect.left);
  const auto windowHeight = static_cast<int>(rect.bottom - rect.top);

  if (config.windowMode != WindowMode::Windowed) {
    style = WS_POPUP;
    styleEx |= WS_EX_TOPMOST;
  }

  const auto windowTitle = create_wide_from_utf8(config.appName);
  sWindowData.handle = ::CreateWindowExW(
    styleEx, WINDOW_CLASS_NAME, windowTitle.c_str(), style,
    CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight, nullptr, nullptr,
    sInstance, nullptr
  );
  if (!sWindowData.handle) {
    throw runtime_error("failed to create window");
  }

  ::ShowWindow(sWindowData.handle, sShowCommand);
  if (config.windowMode != WindowMode::Windowed) {
    ::ShowWindow(sWindowData.handle, SW_SHOWMAXIMIZED);
  }

  ::GetClientRect(sWindowData.handle, &rect);
  sWindowData.clientAreaSize.set(
    static_cast<u16>(rect.right), static_cast<u16>(rect.bottom));

  // TODO: error handling
  sWindowData.factory = D3D9ContextFactory::create().value();
  sWindowData.gfxContext = sWindowData.factory->create_context(sWindowData.handle);
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
  //BASALT_LOG_TRACE("received message: {}", message_to_string(message, wParam, lParam));

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

  case WM_KILLFOCUS:
    // TODO: move somewhere else?
    if (sWindowData.mode != WindowMode::Windowed) {
      ::ShowWindow(sWindowData.handle, SW_MINIMIZE);
    }
    break;

  case WM_ENTERSIZEMOVE:
    sWindowData.isSizing = true;
    return 0;

  case WM_EXITSIZEMOVE:
    sWindowData.isSizing = false;
    sPendingEvents.push_back(
      std::make_shared<WindowResizedEvent>(sWindowData.clientAreaSize)
    );
    return 0;

  case WM_SIZE:
    switch (wParam) {
    case SIZE_RESTORED:
      if (sWindowData.isMinimized) {
        sWindowData.isMinimized = false;
        sPendingEvents.push_back(std::make_shared<WindowRestoredEvent>());
      }
      if (!sWindowData.isSizing) {
        if (const Size2Du16 newSize(LOWORD(lParam), HIWORD(lParam));
          sWindowData.clientAreaSize != newSize) {
          sWindowData.clientAreaSize = newSize;

          sPendingEvents.push_back(
            std::make_shared<WindowResizedEvent>(
              sWindowData.clientAreaSize));
        }
      }
      break;

    case SIZE_MINIMIZED:
      sWindowData.isMinimized = true;
      sPendingEvents.push_back(std::make_shared<WindowMinimizedEvent>());
      break;

    case SIZE_MAXIMIZED:
      if (const Size2Du16 newSize(LOWORD(lParam), HIWORD(lParam));
        sWindowData.clientAreaSize != newSize) {
        sWindowData.clientAreaSize = {LOWORD(lParam), HIWORD(lParam)};
        sPendingEvents.push_back(
          std::make_shared<WindowResizedEvent>(sWindowData.clientAreaSize)
        );
      }
      break;

    default:
      break;
    }

    break;

  case WM_CLOSE:
    sPendingEvents.push_back(std::make_shared<WindowCloseRequestEvent>());
    // DefWindowProc would destroy the window
    return 0;

  case WM_DESTROY:
    sWindowData.gfxContext.reset();
    sWindowData.factory.reset();
    sWindowData.handle = nullptr;
    ::PostQuitMessage(0);
    break;

  default:
    break;
  }

  return ::DefWindowProcW(window, message, wParam, lParam);
}

} // namespace

} // namespace basalt::platform
