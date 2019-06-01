#include "pch.h"

#include <basalt/Engine.h>

#include <chrono>
#include <stdexcept>

//#include <imgui/imgui.h>

#include <basalt/Config.h>
#include <basalt/IApplication.h>
#include <basalt/Input.h>
#include <basalt/Log.h>
#include <basalt/gfx/backend/Factory.h>
#include <basalt/gfx/backend/IRenderer.h>
#include <basalt/platform/Platform.h>

namespace basalt {
namespace {

Config s_config{{"Basalt App", {1280, 720}, WindowMode::FULLSCREEN, false}};
IApplication* s_app;
gfx::backend::IRenderer* s_renderer;


void Startup() {
  BS_INFO("engine startup...");
  BS_INFO("platform: {}", platform::GetName());

  // TODO: load config from file or create default

  s_app = IApplication::Create(s_config);
  if (!s_app) {
    throw std::runtime_error("failed to create IApplication object");
  }

  BS_INFO(
    "config: mainWindow: {} {}x{}{} {}{}",
    s_config.mainWindow.title, s_config.mainWindow.size.GetX(),
    s_config.mainWindow.size.GetY(),
    s_config.mainWindow.mode == WindowMode::FULLSCREEN_BORDERLESS ? " borderless" : "",
    s_config.mainWindow.mode != WindowMode::WINDOWED ? "fullscreen" : "windowed",
    s_config.mainWindow.resizeable ? " resizeable" : ""
  );

  platform::Startup(s_config.mainWindow);

  input::Init();

  s_renderer = gfx::backend::CreateRenderer();
  BS_INFO("gfx backend: {}", s_renderer->GetName());

  /*IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO& io = ImGui::GetIO();
  io.BackendPlatformName = "Basalt";*/

  BS_INFO("engine startup complete");
}


void Shutdown() {
  BS_INFO("shutting down...");

  //ImGui::DestroyContext();

  if (s_renderer) {
    delete s_renderer;
    s_renderer = nullptr;
  }

  gfx::backend::Shutdown();

  platform::Shutdown();

  delete s_app;
  s_app = nullptr;

  BS_INFO("engine shutdown");
}

} // namespace


void Run() {
  Startup();

  s_app->OnInit(s_renderer);

  BS_INFO("entering main loop");
  
  static_assert(std::chrono::high_resolution_clock::is_steady);
  using clock = std::chrono::high_resolution_clock;

  double elapsedTimeInSeconds = 0.0;
  auto startTime = clock::now();
  while (platform::PollEvents()) {
    // update is in between rendering and buffer swapping to utilize the
    // asynchronicity of gpu drivers (gpu does the actual work while updating)
    s_renderer->Render();

    //ImGui::NewFrame();
    s_app->OnUpdate(elapsedTimeInSeconds);
    //ImGui::EndFrame();

    s_renderer->Present();

    auto endTime = clock::now();
    elapsedTimeInSeconds =
      static_cast<double>((endTime - startTime).count())
      / (clock::period::den * clock::period::num);
    startTime = endTime;
  }

  BS_INFO("leaving main loop");

  s_app->OnShutdown();

  Shutdown();
}

} // namespace basalt
