#include <basalt/win32/app.h>

#include <basalt/win32/build_config.h>
#include <basalt/win32/util.h>
#include <basalt/win32/window.h>

#if BASALT_TRACE_WINDOWS_MESSAGES
#include <basalt/win32/debug.h>
#endif // BASALT_TRACE_WINDOWS_MESSAGES

#include <basalt/gfx/backend/d3d9/factory.h>

#include <basalt/dear_imgui.h>

#include <basalt/gfx/compositor.h>
#include <basalt/gfx/debug.h>

#include <basalt/api/client_app.h>
#include <basalt/api/debug.h>
#include <basalt/api/types.h>

#include <basalt/api/gfx/surface.h>
#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/context.h>

#include <basalt/api/shared/config.h>
#include <basalt/api/shared/log.h>
#include <basalt/api/shared/size2d.h>

#include <imgui/imgui.h>

#include <chrono>
#include <string>
#include <string_view>
#include <vector>

using namespace std::literals;

using std::vector;

namespace basalt {

using gfx::Composite;
using gfx::Compositor;
using gfx::D3D9Factory;
using gfx::D3D9FactoryPtr;

namespace {

void dump_config(const Config& config) {
  BASALT_LOG_INFO("config"sv);
  BASALT_LOG_INFO("\truntime.debugUI.enabled = {}"sv,
                  config.get_bool("runtime.debugUI.enabled"s));
  BASALT_LOG_INFO("\tgfx.backend.api = {}"sv,
                  config.get_i32("gfx.backend.api"s));
  BASALT_LOG_INFO("\twindow.title = {}"sv, config.get_string("window.title"s));
  BASALT_LOG_INFO("\twindow.size = {{.width = {}, .height = {}}}"sv,
                  config.get_i32("window.size.width"),
                  config.get_i32("window.size.height"));
  BASALT_LOG_INFO("\twindow.mode = {}"sv, config.get_i32("window.mode"s));
  BASALT_LOG_INFO("\twindow.resizeable = {}"sv,
                  config.get_bool("window.resizeable"s));
}

[[nodiscard]] auto poll_events() -> bool {
  MSG msg {};
  while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
    TranslateMessage(&msg);
    DispatchMessageW(&msg);

    if (!msg.hwnd) {
#if BASALT_TRACE_WINDOWS_MESSAGES
      BASALT_LOG_TRACE("received thread message: {}",
                       message_to_string(msg.message, msg.wParam, msg.lParam));
#endif // BASALT_TRACE_WINDOWS_MESSAGES

      if (msg.message == WM_QUIT) {
        return false;
      }
    }
  }

  return true;
}

[[nodiscard]] auto wait_for_events() -> bool {
  MSG msg {};
  const auto ret = GetMessageW(&msg, nullptr, 0u, 0u);
  if (ret == -1) {
    BASALT_LOG_FATAL(create_win32_error_message(GetLastError()));

    return false;
  }

  // received WM_QUIT
  if (ret == 0) {
    return false;
  }

  TranslateMessage(&msg);
  DispatchMessageW(&msg);

  // handle any remaining messages in the queue
  return poll_events();
}

auto run_lost_device_loop(gfx::Context& gfxContext) -> bool {
  while (wait_for_events()) {
    switch (gfxContext.get_status()) {
    case gfx::ContextStatus::Ok:
      return true;

    case gfx::ContextStatus::Error:
      return false;

    case gfx::ContextStatus::DeviceLost:
      break;

    case gfx::ContextStatus::ResetNeeded:
      gfxContext.reset();

      return true;
    }
  }

  return false;
}

} // namespace

void App::run(Config& config, const HMODULE moduleHandle,
              const int showCommand) {
  dump_config(config);

  const D3D9FactoryPtr gfxFactory {D3D9Factory::create()};
  if (!gfxFactory) {
    BASALT_LOG_FATAL("couldn't create any gfx factory");

    return;
  }

  const Size2Du16 windowedSurfaceSize {
    static_cast<u16>(config.get_i32("window.size.width"s)),
    static_cast<u16>(config.get_i32("window.size.height"s))};
  const Window::Desc windowDesc {
    config.get_string("window.title"s),
    windowedSurfaceSize,
    config.get_enum("window.mode"s, to_window_mode),
    config.get_bool("window.resizeable"s),
  };
  const WindowPtr window {
    Window::create(moduleHandle, showCommand, windowDesc, *gfxFactory)};
  if (!window) {
    BASALT_LOG_FATAL("failed to create window");

    return;
  }

  gfx::Context& gfxContext {window->gfx_context()};

  const gfx::Info& gfxInfo {gfxFactory->info()};

  BASALT_LOG_INFO("Direct3D9 context created: adapter={}, driver={}",
                  gfxInfo.adapters[0].displayName,
                  gfxInfo.adapters[0].driverInfo);

  const auto dearImGui {std::make_shared<DearImGui>(gfxContext.device())};
  ImGuiIO& io {ImGui::GetIO()};
  io.ImeWindowHandle = window->handle();

  window->input_manager().set_overlay(dearImGui);

  App app {config, gfxContext};

  ClientApp::bootstrap(app);

  using Clock = std::chrono::high_resolution_clock;
  static_assert(Clock::is_steady);

  auto startTime {Clock::now()};

  while (poll_events()) {
    if (const WindowMode mode {config.get_enum("window.mode"s, to_window_mode)};
        mode != window->mode()) {
      window->set_mode(mode);
    }

    const vector views {app.mViews};

    window->input_manager().dispatch_pending(views);

    dearImGui->on_tick(app);

    for (auto& view : views) {
      view->on_tick(app);
    }

    if (app.mIsDirty) {
      app.mIsDirty = false;
      window->set_cursor(app.mMouseCursor);
    }

    gfx::Surface surface {gfxContext.surface_size()};
    if (app.mWindowSurfaceContent) {
      surface.draw(app.mWindowSurfaceContent);
    }

    // The DearImGui drawable doesn't actually cause the UI to render during
    // the compositing but is being done at execution of the ExtRenderDearImGui
    // command instead.
    surface.draw(dearImGui);

    Composite composite {Compositor::compose(app.mGfxResourceCache, surface)};

    if (config.get_bool("runtime.debugUI.enabled"s)) {
      Debug::update();
      gfx::Debug::update(gfxInfo, composite);
    }

    gfxContext.submit(composite);
    switch (gfxContext.present()) {
    case gfx::PresentResult::Ok:
      break;
    case gfx::PresentResult::DeviceLost:
      if (!run_lost_device_loop(gfxContext)) {
        quit();
      }

      continue;
    }

    const auto endTime = Clock::now();
    app.mDeltaTime = static_cast<f64>((endTime - startTime).count()) /
                     (Clock::period::den * Clock::period::num);
    startTime = endTime;
  }
}

App::App(Config& config, gfx::Context& gfxContext)
  : Engine {config, gfxContext} {
}

namespace {

///**
// * \brief Processes the windows command line string and populates an argv
// *        style vector.
// *
// * No program name will be added to the array.
// *
// * \param commandLine the windows command line arguments.
// */
// void process_args(const WCHAR* commandLine) {
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

} // namespace basalt
