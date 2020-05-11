#include "runtime/platform/win32/app.h"

#include "runtime/platform/win32/globals.h"
#include "runtime/platform/win32/key_map.h"
#include "runtime/platform/win32/util.h"

#include "runtime/shared/win32/Windows_custom.h"
#include "runtime/shared/win32/util.h"

#include "runtime/dear_imgui.h"
#include "runtime/Engine.h"
#include "runtime/IApplication.h"
#include "runtime/Input.h"

#include "runtime/gfx/backend/d3d9/context_factory.h"
#include "runtime/gfx/backend/context.h"

#include "runtime/platform/Platform.h"

#include "runtime/platform/events/Event.h"
#include "runtime/platform/events/KeyEvents.h"
#include "runtime/platform/events/MouseEvents.h"
#include "runtime/platform/events/WindowEvents.h"

#include "runtime/shared/Asserts.h"
#include "runtime/shared/Log.h"

#include <windowsx.h>

#include <stdexcept>
#include <string>
#include <system_error>
#include <memory>
#include <vector>

using std::runtime_error;
using std::shared_ptr;
using std::string;
using std::system_error;
using std::unique_ptr;
using std::vector;
using std::wstring;

namespace basalt::win32 {

using namespace platform;

using gfx::backend::D3D9ContextFactory;
using gfx::backend::IGfxContext;
using gfx::backend::IRenderer;

vector<PlatformEventCallback> sEventListener;
vector<shared_ptr<Event>> sPendingEvents;
WindowData sWindowData;
unique_ptr<IRenderer> sRenderer {};
shared_ptr<Scene> sCurrentScene {};

namespace {

constexpr auto WINDOW_CLASS_NAME = L"BS_WINDOW_CLASS";

HINSTANCE sInstance;
int sShowCommand;

void dump_config(const Config& config);

void create_main_window(const Config& config);

auto CALLBACK window_proc(
  HWND window, UINT message, WPARAM wParam, LPARAM lParam
) -> LRESULT;

} // namespace

void run(const HINSTANCE instance, const int showCommand) {
  sInstance = instance;
  sShowCommand = showCommand;

  // let the client app configure us
  const auto config {IApplication::configure()};
  dump_config(config);

  create_main_window(config);

  input::init();

  // init imgui before gfx. Renderer initializes imgui render backend
  DearImGui::init();
  sRenderer = sWindowData.gfxContext->create_renderer();

  {
    const auto app = IApplication::create();
    BASALT_ASSERT(app);
    BASALT_ASSERT_MSG(sCurrentScene, "no scene set");

    basalt::run(app.get(), sWindowData.gfxContext.get(), sRenderer.get());
  }

  sRenderer.reset();
  DearImGui::shutdown();

  if (sWindowData.handle) {
    ::DestroyWindow(sWindowData.handle);
    sWindowData.handle = nullptr;
  }

  if (!::UnregisterClassW(WINDOW_CLASS_NAME, sInstance)) {
    BASALT_LOG_ERROR(
      "failed to unregister window class: {}"
    , create_winapi_error_message(::GetLastError()));
  }
}

namespace {

void dump_config(const Config& config) {
  BASALT_LOG_INFO("config");
  BASALT_LOG_INFO("  app name: {}", config.appName);
  BASALT_LOG_INFO(
    "  window: {}x{}{} {}{}"
  , config.windowSize.width(), config.windowSize.height()
  , config.windowMode == WindowMode::FullscreenExclusive ? " exclusive" : ""
  , config.windowMode != WindowMode::Windowed ? "fullscreen" : "windowed"
  , config.isWindowResizeable ? " resizeable" : "");
}

void register_window_class() {
  auto* const cursor = static_cast<HCURSOR>(::LoadImageW(
    nullptr, MAKEINTRESOURCEW(OCR_NORMAL), IMAGE_CURSOR, 0, 0
  , LR_DEFAULTSIZE | LR_SHARED));
  if (!cursor) {
    BASALT_LOG_ERROR("failed to load cursor");
  }

  WNDCLASSEXW windowClass {
    sizeof(WNDCLASSEXW)
  , CS_OWNDC | CS_HREDRAW | CS_VREDRAW
  , &window_proc
  , 0 // cbClsExtra
  , 0 // cbWndExtra
  , sInstance
  , nullptr // hIcon
  , cursor
  , reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1)
  , nullptr // lpszMenuName
  , WINDOW_CLASS_NAME
  , nullptr // hIconSm
  };

  if (!::RegisterClassExW(&windowClass)) {
    throw system_error(
      ::GetLastError(), std::system_category()
    , "Failed to register window class");
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

  const auto windowTitle = win32::create_wide_from_utf8(config.appName);
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
  BASALT_ASSERT(sWindowData.gfxContext);
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

///**
// * \brief Processes the windows command line string and populates an argv
// *        style vector.
// *
// * No program name will be added to the array.
// *
// * \param commandLine the windows command line arguments.
// */
//void process_args(const WCHAR* commandLine) {
//  // check if the command line string is empty to avoid adding
//  // the program name to the argument vector
//  if (commandLine[0] == L'\0') {
//    return;
//  }
//
//  auto argc = 0;
//  auto** argv = ::CommandLineToArgvW(commandLine, &argc);
//  if (argv == nullptr) {
//    // no logging because the log might not be initialized yet
//    return;
//  }
//
//  sArgs.reserve(argc);
//  for (auto i = 0; i < argc; i++) {
//    sArgs.push_back(create_utf8_from_wide(argv[i]));
//  }
//
//  ::LocalFree(argv);
//}

} // namespace

} // namespace basalt::win32
