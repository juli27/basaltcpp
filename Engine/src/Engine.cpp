#include "pch.h"

#include <basalt/Engine.h>

#include <chrono>
#include <memory>
#include <stdexcept>

#include <imgui/imgui.h>

#include <basalt/Config.h>
#include <basalt/IApplication.h>
#include <basalt/Input.h>
#include <basalt/Log.h>
#include <basalt/Scene.h>
#include <basalt/common/Types.h>
#include <basalt/gfx/Gfx.h>
#include <basalt/platform/Platform.h>

namespace basalt {
namespace {

Config sConfig{{"Basalt App", {1280, 720}, WindowMode::FULLSCREEN, false}};
IApplication* sApp;
f64 sCurrentDeltaTime;
std::shared_ptr<Scene> sCurrentScene;


void Startup() {
  BS_INFO("engine startup...");
  BS_INFO("platform: {}", platform::GetName());

  // TODO: load config from file or create default

  sApp = IApplication::Create(sConfig);
  if (!sApp) {
    throw std::runtime_error("failed to create IApplication object");
  }

  BS_INFO(
    "config: window: {} {}x{}{} {}{}",
    sConfig.window.title, sConfig.window.size.GetX(),
    sConfig.window.size.GetY(),
    sConfig.window.mode == WindowMode::FULLSCREEN_EXCLUSIVE ? " exclusive" : "",
    sConfig.window.mode != WindowMode::WINDOWED ? "fullscreen" : "windowed",
    sConfig.window.resizeable ? " resizeable" : ""
  );

  platform::Startup(sConfig.window);
  input::Init();

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  gfx::Init();

  BS_INFO("engine startup complete");
}


void Shutdown() {
  BS_INFO("shutting down...");

  gfx::Shutdown();
  ImGui::DestroyContext();


  platform::Shutdown();

  delete sApp;
  sApp = nullptr;

  BS_INFO("engine shutdown");
}

} // namespace


void Run() {
  Startup();

  // HACK
  sCurrentScene = std::make_shared<Scene>();

  sApp->OnInit();

  BS_INFO("entering main loop");

  static_assert(std::chrono::high_resolution_clock::is_steady);
  using clock = std::chrono::high_resolution_clock;

  auto startTime = clock::now();
  while (platform::PollEvents()) {
    // app update is in between rendering and buffer swapping to utilize the
    // asynchronicity of gpu drivers (gpu does the actual work while updating)
    gfx::Render(sCurrentScene);

    auto& io = ImGui::GetIO();
    const auto windowSize = platform::GetWindowDesc().size;
    io.DisplaySize = ImVec2(
      static_cast<float>(windowSize.GetX()), static_cast<float>(windowSize.GetY())
    );
    io.DeltaTime = static_cast<float>(sCurrentDeltaTime);
    gfx::GetRenderer()->NewGuiFrame();
    io.MousePos = ImVec2(static_cast<float>(input::GetMousePos().GetX()), static_cast<float>(input::GetMousePos().GetY()));
    io.MouseDown[0] = input::IsMouseButtonPressed(input::MouseButton::LEFT);
    io.MouseDown[1] = input::IsMouseButtonPressed(input::MouseButton::RIGHT);
    ImGui::NewFrame();

    sApp->OnUpdate();

    ImGui::Render();
    // gui is 1 frame ahead
    gfx::GetRenderer()->RenderGUI();

    gfx::Present();

    const auto endTime = clock::now();
    sCurrentDeltaTime = static_cast<f64>((endTime - startTime).count()) /
      (clock::period::den * clock::period::num);
    startTime = endTime;
  }

  BS_INFO("leaving main loop");

  sApp->OnShutdown();

  Shutdown();
}


auto GetDeltaTime() -> f64 {
  return sCurrentDeltaTime;
}


void SetCurrentScene(const std::shared_ptr<Scene>& scene) {
  sCurrentScene = scene;
}

} // namespace basalt
