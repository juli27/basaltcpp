#include "pch.h"

#include <basalt/Engine.h>

#include <chrono>
#include <stdexcept>

//#include <imgui/imgui.h>

#include <basalt/Config.h>
#include <basalt/IApplication.h>
#include <basalt/Input.h>
#include <basalt/Log.h>
#include <basalt/common/Types.h>
#include <basalt/gfx/Gfx.h>
#include <basalt/platform/Platform.h>

namespace basalt {
namespace {

Config s_config{{"Basalt App", {1280, 720}, WindowMode::FULLSCREEN, false}};
IApplication* s_app;
f64 s_currentDeltaTime;


void Startup() {
  BS_INFO("engine startup...");
  BS_INFO("platform: {}", platform::GetName());

  // TODO: load config from file or create default

  s_app = IApplication::Create(s_config);
  if (!s_app) {
    throw std::runtime_error("failed to create IApplication object");
  }

  BS_INFO(
    "config: window: {} {}x{}{} {}{}",
    s_config.window.title, s_config.window.size.GetX(),
    s_config.window.size.GetY(),
    s_config.window.mode == WindowMode::FULLSCREEN_EXCLUSIVE ? " exclusive" : "",
    s_config.window.mode != WindowMode::WINDOWED ? "fullscreen" : "windowed",
    s_config.window.resizeable ? " resizeable" : ""
  );

  platform::Startup(s_config.window);
  input::Init();
  gfx::Init();

  /*IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO& io = ImGui::GetIO();
  io.BackendPlatformName = "Basalt";*/

  BS_INFO("engine startup complete");
}


void Shutdown() {
  BS_INFO("shutting down...");

  //ImGui::DestroyContext();

  gfx::Shutdown();

  platform::Shutdown();

  delete s_app;
  s_app = nullptr;

  BS_INFO("engine shutdown");
}

} // namespace


void Run() {
  Startup();

  s_app->OnInit(gfx::GetRenderer());

  BS_INFO("entering main loop");

  static_assert(std::chrono::high_resolution_clock::is_steady);
  using clock = std::chrono::high_resolution_clock;

  auto startTime = clock::now();
  while (platform::PollEvents()) {
    // app update is in between rendering and buffer swapping to utilize the
    // asynchronicity of gpu drivers (gpu does the actual work while updating)
    gfx::Render();

    //ImGui::NewFrame();
    s_app->OnUpdate();
    //ImGui::EndFrame();

    gfx::Present();

    auto endTime = clock::now();
    s_currentDeltaTime = static_cast<f64>((endTime - startTime).count()) /
      (clock::period::den * clock::period::num);
    startTime = endTime;
  }

  BS_INFO("leaving main loop");

  s_app->OnShutdown();

  Shutdown();
}


f64 GetDeltaTime() {
  return s_currentDeltaTime;
}

} // namespace basalt
