#include <basalt/win32/app.h>

#include <basalt/win32/build_config.h>
#include <basalt/win32/types.h>
#include <basalt/win32/util.h>
#include <basalt/win32/window.h>

#if BASALT_TRACE_WINDOWS_MESSAGES
#include <basalt/win32/debug.h>
#endif // BASALT_TRACE_WINDOWS_MESSAGES

#include <basalt/gfx/backend/d3d9/factory.h>

#include <basalt/win32/shared/types.h>
#include <basalt/win32/shared/win32_gfx_factory.h>

#include <basalt/dear_imgui.h>

#include <basalt/gfx/backend/device.h>
#include <basalt/gfx/backend/swap_chain.h>
#include <basalt/gfx/backend/types.h>

#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/shared/config.h>
#include <basalt/api/shared/log.h>
#include <basalt/api/shared/size2d.h>

#include <chrono>
#include <string>
#include <string_view>
#include <utility>

using namespace std::literals;
using std::chrono::duration;
using std::chrono::steady_clock;
using std::chrono::time_point;

namespace basalt {

namespace {

auto dump_config(const Config& config) -> void {
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

auto run_lost_device_loop(gfx::Device& gfxDevice) -> bool {
  while (wait_for_messages()) {
    switch (gfxDevice.get_status()) {
    case gfx::DeviceStatus::Ok:
      return true;

    case gfx::DeviceStatus::Error:
      return false;

    case gfx::DeviceStatus::DeviceLost:
      break;

    case gfx::DeviceStatus::ResetNeeded:
      gfxDevice.reset();

      return true;
    }
  }

  return false;
}

} // namespace

auto App::run(Config& config, const HMODULE moduleHandle, const int showCommand)
  -> void {
  dump_config(config);

  const gfx::Win32GfxFactoryPtr gfxFactory {gfx::D3D9Factory::create()};
  if (!gfxFactory) {
    BASALT_LOG_FATAL("couldn't create any gfx factory");

    return;
  }

  const WindowPtr window {[&] {
    const Size2Du16 windowedSurfaceSize {
      static_cast<u16>(config.get_i32("window.size.width"s)),
      static_cast<u16>(config.get_i32("window.size.height"s)),
    };
    const Window::CreateInfo windowInfo {
      config.get_string("window.title"s),
      showCommand,
      windowedSurfaceSize,
      config.get_enum("window.mode"s, to_window_mode),
      config.get_bool("window.resizeable"s),
    };

    return Window::create(moduleHandle, windowInfo, *gfxFactory);
  }()};

  gfx::Context& gfxContext {*window->gfx_context()};

  App app {
    config,
    window->gfx_context(),
    DearImGui::create(gfxContext, window->handle()),
  };

  window->input_manager().set_overlay(app.dear_imgui());

  using Clock = steady_clock;
  time_point startTime {Clock::now()};
  SecondsF32 deltaTime {0s};

  while (poll_messages()) {
    if (const WindowMode mode {config.get_enum("window.mode"s, to_window_mode)};
        mode != window->mode()) {
      window->set_mode(mode);
    }

    const ViewPtr& rootView {app.root()};

    window->input_manager().dispatch_pending(rootView);

    const UpdateContext runtimeCtx {deltaTime};
    app.update(runtimeCtx);

    if (app.mIsDirty) {
      app.mIsDirty = false;
      window->set_cursor(app.mMouseCursor);
    }

    switch (app.mGfxContext->swap_chain()->present()) {
    case gfx::PresentResult::Ok:
      break;
    case gfx::PresentResult::DeviceLost:
      if (!run_lost_device_loop(*gfxContext.device())) {
        quit();
      }

      continue;
    }

    const time_point endTime {Clock::now()};
    deltaTime = endTime - startTime;
    startTime = endTime;
  }
}

App::App(Config& config, gfx::ContextPtr gfxContext, DearImGuiPtr dearImGui)
  : Runtime {config, std::move(gfxContext), std::move(dearImGui)} {
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
