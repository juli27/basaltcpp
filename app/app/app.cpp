#include "app.h"

#include "build_config.h"
#include "window.h"

#if BASALT_TRACE_WINDOWS_MESSAGES
#include "debug.h"
#endif // BASALT_TRACE_WINDOWS_MESSAGES

#include "d3d9/context.h"
#include "d3d9/factory.h"
#include "d3d9/types.h"
#include "d3d9/util.h"

#include "gfx/debug.h"

#include <api/client_app.h>
#include <api/dear_imgui.h>
#include <api/debug.h>

#include <api/gfx/backend/composite.h>

#include <api/gfx/compositor.h>
#include <api/gfx/draw_target.h>

#include <api/shared/asserts.h>
#include <api/shared/config.h>
#include <api/shared/log.h>
#include <api/shared/size2d.h>

#include <imgui/imgui.h>

#include <chrono>
#include <string>
#include <utility>

using std::shared_ptr;
using std::string;

namespace basalt {

using gfx::AdapterInfo;
using gfx::Composite;
using gfx::Compositor;
using gfx::D3D9Factory;
using gfx::D3D9FactoryPtr;

namespace {

void dump_config(const Config&);

[[nodiscard]] auto poll_events() -> bool;

} // namespace

void App::run(const HMODULE moduleHandle, const int showCommand) {
  // let the client app configure us
  Config config {ClientApp::configure()};
  dump_config(config);

  const auto gfxFactory {D3D9Factory::create()};
  if (!gfxFactory) {
    BASALT_LOG_FATAL("couln't create any gfx factory");

    return;
  }

  const WindowPtr window = Window::create(
    moduleHandle, showCommand, config, gfxFactory->get_current_adapter_mode());
  if (!window) {
    BASALT_LOG_FATAL("failed to create window");

    return;
  }

  const auto [gfxDevice, gfxContext] =
    gfxFactory->create_device_and_context(window->handle(), config);

  const AdapterInfo adapterInfo = gfxFactory->query_adapter_info();

  BASALT_LOG_INFO("Direct3D9 context created: adapter={}, driver={}({})",
                  adapterInfo.displayName, adapterInfo.driver,
                  adapterInfo.driverVersion);

  const auto dearImGui = std::make_shared<DearImGui>(*gfxDevice);
  ImGuiIO& io {ImGui::GetIO()};
  io.ImeWindowHandle = window->handle();

  App app {config, gfxContext};

  const auto clientApp {ClientApp::create(app)};
  BASALT_ASSERT(clientApp);

  using Clock = std::chrono::high_resolution_clock;
  static_assert(Clock::is_steady);

  auto startTime {Clock::now()};
  f64 currentDeltaTime {};

  while (poll_events()) {
    if (const Size2Du16 size = window->client_area_size();
        size != gfxContext->surface_size()) {
      gfxContext->resize(size);
    }

    gfx::DrawTarget drawTarget {gfxContext->surface_size()};

    const UpdateContext ctx {app, drawTarget, currentDeltaTime,
                             window->drain_input()};
    dearImGui->new_frame(ctx);

    clientApp->on_update(ctx);

    if (app.mIsDirty) {
      app.mIsDirty = false;
      window->set_cursor(app.mMouseCursor);

      if (window->current_mode() != config.windowMode) {
        window->set_mode(config.windowMode,
                         gfxFactory->get_current_adapter_mode());
      }
    }

    if (config.debugUiEnabled) {
      Debug::update();
      gfx::Debug::update(adapterInfo);
    }

    drawTarget.draw(dearImGui);

    // device needed for our current model support
    const Composite composite =
      Compositor::compose(app.mGfxResourceCache, drawTarget);
    Debug::update(composite);

    gfxContext->submit(composite);
    gfxContext->present();

    const auto endTime = Clock::now();
    currentDeltaTime = static_cast<f64>((endTime - startTime).count()) /
                       (Clock::period::den * Clock::period::num);
    startTime = endTime;
  }
}

App::App(Config& config, shared_ptr<gfx::Context> gfxContext)
  : Engine {config, std::move(gfxContext)} {
}

namespace {

void dump_config(const Config& config) {
  BASALT_LOG_INFO("config");
  BASALT_LOG_INFO("\tapp name: {}", config.appName);
  BASALT_LOG_INFO(
    "\twindow: {}x{}{} {}{}", config.windowedSize.width(),
    config.windowedSize.height(),
    config.windowMode == WindowMode::FullscreenExclusive ? " exclusive" : "",
    config.windowMode != WindowMode::Windowed ? "fullscreen" : "windowed",
    config.isWindowResizeable ? " resizeable" : "");
}

auto poll_events() -> bool {
  MSG msg {};
  while (::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
    ::TranslateMessage(&msg);
    ::DispatchMessageW(&msg);

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

// auto wait_for_events() -> vector<shared_ptr<Event>> {
//  MSG msg{};
//  const auto ret = ::GetMessageW(&msg, nullptr, 0u, 0u);
//  if (ret == -1) {
//    BASALT_LOG_ERROR(create_win32_error_message(::GetLastError()));
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
