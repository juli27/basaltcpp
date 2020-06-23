#include "app.h"

#include "build_config.h"
#include "globals.h"
#include "window.h"

#if BASALT_TRACE_WINDOWS_MESSAGES
#include "debug.h"
#endif // BASALT_TRACE_WINDOWS_MESSAGES

#include "d3d9/factory.h"
#include "d3d9/types.h"
#include "d3d9/util.h"

#include <runtime/client_app.h>
#include <runtime/dear_imgui.h>

#include <runtime/gfx/compositor.h>

#include <runtime/shared/asserts.h>
#include <runtime/shared/log.h>
#include <runtime/shared/size2d.h>

#include <imgui/imgui.h>

#include <chrono>
#include <string>
#include <memory>
#include <utility>
#include <vector>

using std::string;

namespace basalt {

using gfx::Compositor;
using gfx::AdapterInfo;
using gfx::D3D9Factory;

namespace {

void dump_config(const Config& config);

[[nodiscard]]
auto poll_events() -> bool;

void draw_debug_ui_additional(const D3D9Factory&);

} // namespace

void App::run(const HMODULE moduleHandle, const int showCommand) {
  // let the client app configure us
  Config config {ClientApp::configure()};
  dump_config(config);

  // creates the window, the associated gfx context and the device
  const WindowPtr window {Window::create(moduleHandle, showCommand, config)};

  const DearImGui dearImGui {window->gfx_device()};
  ImGuiIO& io {ImGui::GetIO()};
  io.ImeWindowHandle = window->handle();

  App app {config, *window};
  Compositor compositor {window->gfx_context()};

  const auto clientApp {ClientApp::create(app)};
  BASALT_ASSERT(clientApp);

  using Clock = std::chrono::high_resolution_clock;
  static_assert(Clock::is_steady);
  auto startTime {Clock::now()};
  f64 currentDeltaTime {0.0};

  while (poll_events()) {
    const UpdateContext ctx {
      app, compositor.draw_target(), currentDeltaTime, window->drain_input()
    };
    dearImGui.new_frame(ctx);

    clientApp->on_update(ctx);

    if (app.mIsDirty) {
      app.mIsDirty = false;
      window->set_cursor(app.mMouseCursor);
    }

    draw_debug_ui_additional(*window->context_factory());

    // also calls ImGui::Render()
    compositor.compose();

    window->present();

    const auto endTime = Clock::now();
    currentDeltaTime = static_cast<f64>((endTime - startTime).count()) / (
      Clock::period::den * Clock::period::num);
    startTime = endTime;
  }
}

App::App(Config& config, const Window& window)
  : Engine {config, window.gfx_context()} {
}

namespace {

void dump_config(const Config& config) {
  BASALT_LOG_INFO("config");
  BASALT_LOG_INFO("  app name: {}", config.appName);
  BASALT_LOG_INFO(
    "  window: {}x{}{} {}{}"
  , config.windowedSize.width(), config.windowedSize.height()
  , config.windowMode == WindowMode::FullscreenExclusive ? " exclusive" : ""
  , config.windowMode != WindowMode::Windowed ? "fullscreen" : "windowed"
  , config.isWindowResizeable ? " resizeable" : "");
}

auto poll_events() -> bool {
  MSG msg {};
  while (::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
    ::TranslateMessage(&msg);
    ::DispatchMessageW(&msg);

    if (!msg.hwnd) {
#if BASALT_TRACE_WINDOWS_MESSAGES
      BASALT_LOG_TRACE(
        "received thread message: {}"
      , message_to_string(msg.message, msg.wParam, msg.lParam));
#endif // BASALT_TRACE_WINDOWS_MESSAGES

      if (msg.message == WM_QUIT) {
        return false;
      }
    }
  }

  return true;
}

void draw_debug_ui_additional(const D3D9Factory& ctxFactory) {
  // https://github.com/ocornut/imgui/issues/331
  enum class OpenPopup : u8 {
    None, GfxInfo
  };
  OpenPopup shouldOpenPopup {OpenPopup::None};

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("View")) {
      ImGui::Separator();

      if (ImGui::MenuItem("GFX Info...")) {
        shouldOpenPopup = OpenPopup::GfxInfo;
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }

  if (shouldOpenPopup == OpenPopup::GfxInfo) {
    ImGui::OpenPopup("Gfx Info");
  }

  if (ImGui::BeginPopupModal(
    "Gfx Info", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
    const AdapterInfo& adapterInfo = ctxFactory.adapter_info();

    ImGui::Text(
      "GFX Adapter: %s", adapterInfo.displayName.c_str());
    ImGui::Text(
      "Driver: %s (%s)", adapterInfo.driver.c_str()
    , adapterInfo.driverVersion.c_str());

    static string current = fmt::format(
      "{}x{} {}Hz {}", adapterInfo.defaultAdapterMode.width
    , adapterInfo.defaultAdapterMode.height
    , adapterInfo.defaultAdapterMode.refreshRate, to_string(
        adapterInfo.defaultAdapterMode.displayFormat));

    if (ImGui::BeginCombo("Adapter Modes", current.c_str())) {
      for (const auto& adapterMode : adapterInfo.adapterModes) {
        string rep {
          fmt::format(
            "{}x{} {}Hz {}", adapterMode.width, adapterMode.height
          , adapterMode.refreshRate, to_string(adapterMode.displayFormat))
        };

        const bool isSelected = current == rep;

        if (ImGui::Selectable(rep.c_str(), isSelected)) {
          current = std::move(rep);
        }

        if (isSelected) {
          ImGui::SetItemDefaultFocus();
        }
      }

      ImGui::EndCombo();
    }

    if (ImGui::Button("OK", ImVec2 {120.0f, 0.0f})) {
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }
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

} // namespace basalt
