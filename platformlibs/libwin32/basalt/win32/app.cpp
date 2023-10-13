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

#include <basalt/api/bootstrap.h>

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

auto dump_config(Config const& config) -> void {
  BASALT_LOG_INFO("config"sv);
  BASALT_LOG_INFO("\truntime.debugUI.enabled = {}"sv,
                  config.get_bool("runtime.debugUI.enabled"s));
}

[[nodiscard]]
auto create_gfx_factory(gfx::BackendApi const backendApi)
  -> gfx::Win32GfxFactoryPtr {
  switch (backendApi) {
  case gfx::BackendApi::Default:
  case gfx::BackendApi::Direct3D9:
    return gfx::D3D9Factory::create();
  }

  BASALT_CRASH("unsupported gfx backend api");
}

[[nodiscard]]
auto poll_messages() -> bool {
  auto msg = MSG{};
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

[[nodiscard]]
auto wait_for_messages() -> bool {
  auto msg = MSG{};
  auto const ret = GetMessageW(&msg, nullptr, 0u, 0u);
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

auto App::run(HMODULE const moduleHandle, int const showCommand) -> void {
  auto clientApp = bootstrap_app();
  auto& config = clientApp.config;
  dump_config(config);

  auto const gfxFactory = create_gfx_factory(clientApp.gfxBackendApi);
  if (!gfxFactory) {
    BASALT_CRASH("couldn't create any gfx factory");
  }

  auto const window = [&] {
    auto const windowInfo = Window::CreateInfo{
      clientApp.windowTitle,        showCommand,
      clientApp.canvasSize,         clientApp.windowMode,
      clientApp.isCanvasResizeable,
    };

    return Window::create(moduleHandle, windowInfo, *gfxFactory);
  }();

  auto& gfxContext = *window->gfx_context();

  auto app = App{
    config,
    window->gfx_context(),
    DearImGui::create(gfxContext, window->handle()),
  };

  window->input_manager().set_overlay(app.dear_imgui());
  app.set_root(clientApp.createRootView(app));

  using Clock = steady_clock;
  auto startTime = Clock::now();
  auto deltaTime = SecondsF32{0s};

  while (poll_messages()) {
    if (auto const mode = config.get_enum("window.mode"s, to_window_mode);
        mode != window->mode()) {
      window->set_mode(mode);
    }

    window->input_manager().dispatch_pending(app.root());

    auto const runtimeCtx = UpdateContext{deltaTime};
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

    auto const endTime = Clock::now();
    deltaTime = endTime - startTime;
    startTime = endTime;
  }
}

App::App(Config& config, gfx::ContextPtr gfxContext, DearImGuiPtr dearImGui)
  : Runtime{config, std::move(gfxContext), std::move(dearImGui)} {
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
