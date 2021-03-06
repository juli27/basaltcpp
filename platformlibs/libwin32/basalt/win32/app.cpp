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

#include <basalt/api/gfx/draw_target.h>
#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/context.h>

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/config.h>
#include <basalt/api/shared/log.h>
#include <basalt/api/shared/size2d.h>

#include <imgui/imgui.h>

#include <chrono>
#include <string>
#include <utility>

using namespace std::literals;

using std::shared_ptr;
using std::string;

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
  BASALT_LOG_INFO("\twindow.surface.windowedSize = "
                  "{{.width = {}, .height = {}}}"sv,
                  config.get_i32("window.surface.windowedSize.width"),
                  config.get_i32("window.surface.windowedSize.height"));
  BASALT_LOG_INFO("\twindow.mode = {}"sv, config.get_i32("window.mode"s));
  BASALT_LOG_INFO("\twindow.resizeable = {}"sv,
                  config.get_bool("window.resizeable"s));
}

auto build_gfx_info(const D3D9Factory& factory) -> gfx::Info {
  gfx::Info gfxInfo {};
  const u32 adapterCount = factory.get_adapter_count();
  gfxInfo.adapters.reserve(adapterCount);
  for (u32 i = 0; i < adapterCount; ++i) {
    gfxInfo.adapters.emplace_back(factory.query_adapter_info(i));
  }

  return gfxInfo;
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

  const auto gfxFactory {D3D9Factory::create()};
  if (!gfxFactory) {
    BASALT_LOG_FATAL("couldn't create any gfx factory");

    return;
  }

  const Size2Du16 windowedSurfaceSize {
    static_cast<u16>(config.get_i32("window.surface.windowedSize.width"s)),
    static_cast<u16>(config.get_i32("window.surface.windowedSize.height"s))};
  const Window::Desc windowDesc {
    config.get_string("window.title"s), windowedSurfaceSize,
    config.get_enum("window.mode"s, to_window_mode),
    config.get_bool("window.resizeable"s)};
  const WindowPtr window {
    Window::create(moduleHandle, showCommand, windowDesc)};
  if (!window) {
    BASALT_LOG_FATAL("failed to create window");

    return;
  }

  D3D9Factory::DeviceAndContextDesc deviceAndContextDesc {};
  deviceAndContextDesc.exclusive =
    window->mode() == WindowMode::FullscreenExclusive;

  const auto [gfxDevice, gfxContext] = gfxFactory->create_device_and_context(
    window->handle(), deviceAndContextDesc);

  const gfx::Info gfxInfo {build_gfx_info(*gfxFactory)};

  BASALT_LOG_INFO("Direct3D9 context created: adapter={}, driver={}",
                  gfxInfo.adapters[0].displayName,
                  gfxInfo.adapters[0].driverInfo);

  const auto dearImGui {std::make_shared<DearImGui>(*gfxDevice)};
  ImGuiIO& io {ImGui::GetIO()};
  io.ImeWindowHandle = window->handle();

  window->input_manager().set_overlay(dearImGui);

  App app {config, gfxContext};

  const auto clientApp {ClientApp::create(app)};
  BASALT_ASSERT(clientApp);

  using Clock = std::chrono::high_resolution_clock;
  static_assert(Clock::is_steady);

  auto startTime {Clock::now()};
  f64 currentDeltaTime {};

  while (poll_events()) {
    if (const Size2Du16 size {window->client_area_size()};
        config.get_enum("window.mode"s, to_window_mode) != window->mode() ||
        size != gfxContext->surface_size() &&
          window->mode() == WindowMode::Windowed) {
      // call Window::set_mode after resetting the context when leaving
      // exclusive mode because the D3D9 runtime handles all exclusive mode
      // window changes
      if (window->mode() == WindowMode::FullscreenExclusive) {
        gfx::Context::ResetDesc desc {};
        gfxContext->reset(desc);

        window->set_mode(config.get_enum("window.mode"s, to_window_mode));
      } else {
        const WindowMode mode = config.get_enum("window.mode"s, to_window_mode);
        window->set_mode(mode);

        gfx::Context::ResetDesc desc {};
        desc.windowBackBufferSize = window->client_area_size();
        desc.exclusive = mode == WindowMode::FullscreenExclusive;
        gfxContext->reset(desc);
      }
    }

    window->input_manager().dispatch_pending(app.mInputLayers);

    gfx::DrawTarget drawTarget {gfxContext->surface_size()};

    const UpdateContext ctx {app, drawTarget, currentDeltaTime};
    dearImGui->new_frame(ctx);

    clientApp->on_update(ctx);

    if (app.mIsDirty) {
      app.mIsDirty = false;
      window->set_cursor(app.mMouseCursor);
    }

    // The DearImGui drawable doesn't actually cause the UI to render during
    // the compositing but is being done at execution of the ExtRenderDearImGui
    // command instead.
    drawTarget.draw(dearImGui);

    const Composite composite =
      Compositor::compose(app.mGfxResourceCache, drawTarget);

    if (config.get_bool("runtime.debugUI.enabled"s)) {
      Debug::update();
      gfx::Debug::update(gfxInfo, composite);
    }

    gfxContext->submit(composite);
    switch (gfxContext->present()) {
    case gfx::PresentResult::Ok:
      break;
    case gfx::PresentResult::DeviceLost:
      if (!run_lost_device_loop(*gfxContext)) {
        quit();
      }

      continue;
    }

    const auto endTime = Clock::now();
    currentDeltaTime = static_cast<f64>((endTime - startTime).count()) /
                       (Clock::period::den * Clock::period::num);
    startTime = endTime;
  }
}

App::App(Config& config, gfx::ContextPtr gfxContext)
  : Engine {config, std::move(gfxContext)} {
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
