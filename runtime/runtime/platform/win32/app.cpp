#include "runtime/platform/win32/app.h"

#include "runtime/platform/win32/globals.h"
#include "runtime/platform/win32/key_map.h"
//#include "runtime/platform/win32/messages.h"
#include "runtime/platform/win32/util.h"

#include "runtime/shared/win32/Windows_custom.h"
#include "runtime/shared/win32/util.h"

#include "runtime/dear_imgui.h"
#include "runtime/IApplication.h"
#include "runtime/Input.h"

#include "runtime/gfx/Gfx.h"

#include "runtime/gfx/backend/d3d9/context_factory.h"
#include "runtime/gfx/backend/d3d9/context.h"

#include "runtime/platform/Platform.h"

#include "runtime/platform/events/Event.h"
#include "runtime/platform/events/KeyEvents.h"
#include "runtime/platform/events/MouseEvents.h"

#include "runtime/shared/Asserts.h"
#include "runtime/shared/Log.h"
#include "runtime/shared/Size2D.h"

#include <windowsx.h>

#include <chrono>
#include <stdexcept>
#include <string>
#include <system_error>
#include <memory>
#include <utility>
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
using gfx::backend::D3D9GfxContext;
using gfx::backend::IRenderer;

vector<PlatformEventCallback> sEventListener;
WindowData sWindowData;
shared_ptr<Scene> sCurrentScene {};

namespace {

struct Window final {
  Window() = delete;

  Window(const Window&) = delete;
  Window(Window&&) = delete;

  ~Window();

  auto operator=(const Window&) -> Window& = delete;
  auto operator=(Window&&) -> Window& = delete;

  void present() const {
    mContext->present();
  }

  [[nodiscard]]
  auto renderer() const -> IRenderer* {
    return mRenderer.get();
  }

  [[nodiscard]]
  static auto create(
    HINSTANCE instance, int showCommand, const Config& config
  ) -> unique_ptr<Window>;

private:
  static constexpr auto CLASS_NAME = L"BS_WINDOW_CLASS";

  HINSTANCE mInstance {nullptr};
  HWND mHandle {nullptr};
  unique_ptr<D3D9ContextFactory> mFactory {};
  unique_ptr<D3D9GfxContext> mContext {};
  unique_ptr<IRenderer> mRenderer {};
  bool mInSizingMode {false};

  Window(
    HINSTANCE instance, HWND handle, unique_ptr<D3D9ContextFactory> factory
  , unique_ptr<D3D9GfxContext> context, unique_ptr<IRenderer> renderer
  );

  [[nodiscard]]
  auto dispatch_message(UINT message, WPARAM, LPARAM) -> LRESULT;

  void resize(Size2Du16 clientArea) const;

  static void register_class(HINSTANCE instance);

  static auto CALLBACK window_proc(
    HWND, UINT message, WPARAM, LPARAM
  ) -> LRESULT;
};

void dump_config(const Config& config);

[[nodiscard]]
auto poll_events() -> bool;

} // namespace

void run(const HINSTANCE instance, const int showCommand) {
  // let the client app configure us
  const auto config {IApplication::configure()};
  dump_config(config);

  // init imgui before gfx. Renderer initializes imgui render backend
  DearImGui::init();

  {
    // creates the window, the associated gfx context and the renderer
    const auto window = Window::create(instance, showCommand, config);
    input::init();

    const auto clientApp = IApplication::create(window->renderer());
    BASALT_ASSERT(clientApp);
    BASALT_ASSERT_MSG(sCurrentScene, "no scene set");

    static_assert(std::chrono::high_resolution_clock::is_steady);
    using Clock = std::chrono::high_resolution_clock;
    auto startTime = Clock::now();
    auto currentDeltaTime = 0.0;

    do {
      DearImGui::new_frame(window->renderer(), currentDeltaTime);

      clientApp->on_update(currentDeltaTime);

      // also calls ImGui::Render()
      gfx::render(window->renderer(), sCurrentScene.get());

      window->present();

      const auto endTime = Clock::now();
      currentDeltaTime = static_cast<f64>((endTime - startTime).count()) / (
        Clock::period::den * Clock::period::num);
      startTime = endTime;
    } while (poll_events());

    sCurrentScene.reset();
  }

  DearImGui::shutdown();
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

Window::~Window() {
  if (!::DestroyWindow(mHandle)) {
    BASALT_LOG_ERROR(
      "::DestroyWindow failed: {}"
    , create_winapi_error_message(::GetLastError()));
  }

  if (!::UnregisterClassW(CLASS_NAME, mInstance)) {
    BASALT_LOG_ERROR(
      "::UnregisterClassW failed: {}"
    , create_winapi_error_message(::GetLastError()));
  }
}

auto Window::create(
  const HINSTANCE instance, const int showCommand, const Config& config
) -> unique_ptr<Window> {
  register_class(instance);

  sWindowData.mode = config.windowMode;

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
  auto* const handle = ::CreateWindowExW(
    styleEx, CLASS_NAME, windowTitle.c_str(), style,
    CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight, nullptr, nullptr,
    instance, nullptr
  );
  if (!handle) {
    throw runtime_error("failed to create window");
  }

  // TODO: error handling
  auto factory = D3D9ContextFactory::create().value();
  auto gfxContext = factory->create_context(handle);
  auto renderer = gfxContext->create_renderer();

  auto* const window = new Window {
    instance, handle, std::move(factory), std::move(gfxContext)
  , std::move(renderer)
  };

  // unique_ptr is actually a lie
  // see the comment at the WM_CLOSE message for details
  ::SetWindowLongPtrW(
    handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));

  ::ShowWindow(handle, showCommand);
  if (config.windowMode != WindowMode::Windowed) {
    ::ShowWindow(handle, SW_SHOWMAXIMIZED);
  }

  ::GetClientRect(handle, &rect);
  sWindowData.clientAreaSize.set(
    static_cast<u16>(rect.right), static_cast<u16>(rect.bottom));

  return unique_ptr<Window> {window};
}

void dispatch_platform_event(const Event& event) {
  std::for_each(
    sEventListener.cbegin(), sEventListener.cend(),
    [&event](const PlatformEventCallback& callback) {
      callback(event);
    }
  );
}

Window::Window(
  const HINSTANCE instance, const HWND handle
, unique_ptr<D3D9ContextFactory> factory, unique_ptr<D3D9GfxContext> context
, unique_ptr<IRenderer> renderer
)
  : mInstance {instance}
  , mHandle {handle}
  , mFactory {std::move(factory)}
  , mContext {std::move(context)}
  , mRenderer {std::move(renderer)} {
  BASALT_ASSERT(mInstance);
  BASALT_ASSERT(mHandle);
  BASALT_ASSERT(mFactory);
  BASALT_ASSERT(mContext);
  BASALT_ASSERT(mRenderer);
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
          sWindowData.clientAreaSize != newSize) {
          sWindowData.clientAreaSize = newSize;

          resize(sWindowData.clientAreaSize);
        }
      }
      break;

    case SIZE_MAXIMIZED:
      if (const Size2Du16 newSize(LOWORD(lParam), HIWORD(lParam));
        sWindowData.clientAreaSize != newSize) {
        sWindowData.clientAreaSize = {LOWORD(lParam), HIWORD(lParam)};
        resize(sWindowData.clientAreaSize);
      }
      break;

    default:
      break;
    }
    break;

  case WM_KILLFOCUS:
    // TODO: move somewhere else?
    if (sWindowData.mode != WindowMode::Windowed) {
      ::ShowWindow(mHandle, SW_MINIMIZE);
    }
    break;

  case WM_CLOSE:
    // ::DefWindowProcW would destroy the window.
    // In order to not lie about the lifetime of the window object, we handle
    // the WM_CLOSE message here and post the quit message to trigger our normal
    // shutdown path
    ::PostQuitMessage(0);
    return 0;

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

  case WM_MOUSEMOVE:
    dispatch_platform_event(
      MouseMovedEvent({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)})
    );
    return 0;

  case WM_LBUTTONDOWN:
    ::SetCapture(mHandle);
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

  case WM_MOUSEWHEEL: {
    const auto offset = GET_WHEEL_DELTA_WPARAM(wParam) / static_cast<f32>(
      WHEEL_DELTA);
    dispatch_platform_event(MouseWheelScrolledEvent(offset));
    return 0;
  }

  case WM_ENTERSIZEMOVE:
    mInSizingMode = true;
    return 0;

  case WM_EXITSIZEMOVE:
    mInSizingMode = false;
    resize(sWindowData.clientAreaSize);
    return 0;

  default:
    break;
  }

  return ::DefWindowProcW(mHandle, message, wParam, lParam);
}

void Window::resize(const Size2Du16 clientArea) const {
  mRenderer->on_window_resize(clientArea);
}

void Window::register_class(const HINSTANCE instance) {
  auto* const cursor = static_cast<HCURSOR>(::LoadImageW(
    nullptr, MAKEINTRESOURCEW(OCR_NORMAL), IMAGE_CURSOR, 0, 0
  , LR_DEFAULTSIZE | LR_SHARED));
  if (!cursor) {
    BASALT_LOG_ERROR("failed to load cursor");
  }

  WNDCLASSEXW windowClass {
    sizeof(WNDCLASSEXW)
  , CS_OWNDC | CS_HREDRAW | CS_VREDRAW
  , &Window::window_proc
  , 0 // cbClsExtra
  , 0 // cbWndExtra
  , instance
  , nullptr // hIcon
  , cursor
  , reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1)
  , nullptr // lpszMenuName
  , CLASS_NAME
  , nullptr // hIconSm
  };

  if (!::RegisterClassExW(&windowClass)) {
    throw system_error(
      ::GetLastError(), std::system_category()
    , "Failed to register window class");
  }
}

auto CALLBACK Window::window_proc(
  const HWND handle, const UINT message, const WPARAM wParam
, const LPARAM lParam
) -> LRESULT {
  /*BASALT_LOG_TRACE(
    "received message: {}", message_to_string(message, wParam, lParam));*/

  if (const auto window = ::GetWindowLongPtrW(handle, GWLP_USERDATA)) {
    return reinterpret_cast<Window*>(window)->dispatch_message(
      message, wParam, lParam);
  }

  return ::DefWindowProcW(handle, message, wParam, lParam);
}

auto poll_events() -> bool {
  MSG msg {};
  while (::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
    ::TranslateMessage(&msg);
    ::DispatchMessageW(&msg);

    if (!msg.hwnd) {
      switch (msg.message) {
      case WM_QUIT:
        return false;

      default:
        // 275 is WM_TIMER
        // is received upon focus change
        BASALT_LOG_DEBUG("unhandled thread message: {}", msg.message);
        break;
      }
    }
  }

  return true;
}

//auto wait_for_events() -> vector<shared_ptr<Event>> {
//  MSG msg{};
//  const auto ret = ::GetMessageW(&msg, nullptr, 0u, 0u);
//  if (ret == -1) {
//    BASALT_LOG_ERROR(create_winapi_error_message(::GetLastError()));
//    // TODO: fixme
//    BASALT_ASSERT_MSG(false, "::GetMessageW error");
//  }
//
//  // GetMessage retrieved WM_QUIT
//  if (ret == 0) {
//    sPendingEvents.push_back(std::make_shared<QuitEvent>());
//    return sPendingEvents;
//  }
//
//  ::TranslateMessage(&msg);
//  ::DispatchMessageW(&msg);
//
//  // handle any remainig messages in the queue
//  return poll_events();
//}

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
