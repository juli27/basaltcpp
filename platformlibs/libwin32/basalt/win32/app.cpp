#include <basalt/win32/app.h>

#include <basalt/win32/build_config.h>
#include <basalt/win32/types.h>
#include <basalt/win32/util.h>
#include <basalt/win32/window.h>

#if BASALT_TRACE_WINDOWS_MESSAGES
#include <basalt/win32/debug.h>
#endif // BASALT_TRACE_WINDOWS_MESSAGES

#include <basalt/dear_imgui.h>

#include <basalt/gfx/backend/d3d9/factory.h>

#include <basalt/gfx/backend/context.h>
#include <basalt/gfx/backend/types.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/shared/config.h>
#include <basalt/api/shared/log.h>
#include <basalt/api/shared/size2d.h>

#include <imgui/imgui.h>

#include <chrono>
#include <string>
#include <string_view>
#include <utility>

using namespace std::literals;

namespace basalt {

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

[[nodiscard]] auto poll_messages() -> bool {
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

[[nodiscard]] auto wait_for_messages() -> bool {
  MSG msg {};
  const BOOL ret {GetMessageW(&msg, nullptr, 0u, 0u)};
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
  return poll_messages();
}

auto run_lost_device_loop(gfx::Context& gfxContext) -> bool {
  while (wait_for_messages()) {
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

  const gfx::D3D9FactoryPtr gfxFactory {gfx::D3D9Factory::create()};
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

  const gfx::ContextPtr& gfxContext {window->gfx_context()};
  const gfx::DevicePtr gfxDevice {gfxContext->device()};

  const auto dearImGui {std::make_shared<DearImGui>(*gfxDevice)};
  ImGuiIO& io {ImGui::GetIO()};
  io.ImeWindowHandle = window->handle();

  window->input_manager().set_overlay(dearImGui);

  App app {
    config,
    gfx::Info {
      gfxDevice->capabilities(),
      gfxFactory->adapters(),
      gfx::BackendApi::Direct3D9,
    },
    gfxContext,
    dearImGui,
  };

  using Clock = std::chrono::high_resolution_clock;
  static_assert(Clock::is_steady);

  auto startTime {Clock::now()};

  while (poll_messages()) {
    if (const WindowMode mode {config.get_enum("window.mode"s, to_window_mode)};
        mode != window->mode()) {
      window->set_mode(mode);
    }

    const ViewPtr& rootView {app.root()};

    window->input_manager().dispatch_pending(rootView);

    app.tick();

    if (app.mIsDirty) {
      app.mIsDirty = false;
      window->set_cursor(app.mMouseCursor);
    }

    app.render();

    switch (gfxContext->present()) {
    case gfx::PresentResult::Ok:
      break;
    case gfx::PresentResult::DeviceLost:
      if (!run_lost_device_loop(*gfxContext)) {
        quit();
      }

      continue;
    }

    const auto endTime {Clock::now()};
    app.mDeltaTime = static_cast<f64>((endTime - startTime).count()) /
                     (Clock::period::den * Clock::period::num);
    startTime = endTime;
  }
}

App::App(Config& config, gfx::Info gfxInfo, gfx::ContextPtr gfxContext,
         DearImGuiPtr dearImGui)
  : Runtime {config, std::move(gfxInfo), std::move(gfxContext),
             std::move(dearImGui)} {
}

// namespace {
//
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
//
//} // namespace

} // namespace basalt
