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

Config sConfig{{"Basalt App", {1280, 720}, WindowMode::Fullscreen, false}};
IApplication* sApp;
f64 sCurrentDeltaTime;
std::shared_ptr<Scene> sCurrentScene;


void InitDearImGui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  auto& imguiIo = ImGui::GetIO();
  imguiIo.ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard;
  imguiIo.KeyMap[ImGuiKey_Tab] = static_cast<i8>(input::Key::Tab);
  imguiIo.KeyMap[ImGuiKey_LeftArrow] = static_cast<i8>(input::Key::LeftArrow);
  imguiIo.KeyMap[ImGuiKey_RightArrow] = static_cast<i8>(input::Key::RightArrow);
  imguiIo.KeyMap[ImGuiKey_UpArrow] = static_cast<i8>(input::Key::UpArrow);
  imguiIo.KeyMap[ImGuiKey_DownArrow] = static_cast<i8>(input::Key::DownArrow);
  imguiIo.KeyMap[ImGuiKey_PageUp] = static_cast<i8>(input::Key::PageUp);
  imguiIo.KeyMap[ImGuiKey_PageDown] = static_cast<i8>(input::Key::PageDown);
  imguiIo.KeyMap[ImGuiKey_Home] = static_cast<i8>(input::Key::Home);
  imguiIo.KeyMap[ImGuiKey_End] = static_cast<i8>(input::Key::End);
  imguiIo.KeyMap[ImGuiKey_Insert] = static_cast<i8>(input::Key::Insert);
  imguiIo.KeyMap[ImGuiKey_Delete] = static_cast<i8>(input::Key::Delete);
  imguiIo.KeyMap[ImGuiKey_Backspace] = static_cast<i8>(input::Key::Backspace);
  imguiIo.KeyMap[ImGuiKey_Space] = static_cast<i8>(input::Key::Space);
  imguiIo.KeyMap[ImGuiKey_Enter] = static_cast<i8>(input::Key::Enter);
  imguiIo.KeyMap[ImGuiKey_Escape] = static_cast<i8>(input::Key::Escape);
  imguiIo.KeyMap[ImGuiKey_KeyPadEnter] =
    static_cast<i8>(input::Key::NumpadEnter);
  imguiIo.KeyMap[ImGuiKey_A] = static_cast<i8>(input::Key::A);
  imguiIo.KeyMap[ImGuiKey_C] = static_cast<i8>(input::Key::C);
  imguiIo.KeyMap[ImGuiKey_V] = static_cast<i8>(input::Key::V);
  imguiIo.KeyMap[ImGuiKey_X] = static_cast<i8>(input::Key::X);
  imguiIo.KeyMap[ImGuiKey_Y] = static_cast<i8>(input::Key::Y);
  imguiIo.KeyMap[ImGuiKey_Z] = static_cast<i8>(input::Key::Z);

  platform::AddEventListener([](const platform::Event& e) {
    const platform::EventDispatcher dispatcher(e);
    auto& io = ImGui::GetIO();
    dispatcher.Dispatch<platform::KeyPressedEvent>(
      [&](const platform::KeyPressedEvent& event) {
      io.KeysDown[static_cast<i8>(event.mKey)] = true;
    });
    dispatcher.Dispatch<platform::KeyReleasedEvent>(
      [&](const platform::KeyReleasedEvent& event) {
      io.KeysDown[static_cast<i8>(event.mKey)] = false;
    });
    dispatcher.Dispatch<platform::CharactersTyped>(
      [&](const platform::CharactersTyped& event) {
      io.AddInputCharactersUTF8(event.mChars.c_str());
    });
    dispatcher.Dispatch<platform::MouseWheelScrolledEvent>(
      [&](const platform::MouseWheelScrolledEvent& event) {
      io.MouseWheel = event.mOffset;
    });
  });
}


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
    sConfig.mWindow.mTitle, sConfig.mWindow.mSize.GetX(),
    sConfig.mWindow.mSize.GetY(),
    sConfig.mWindow.mMode == WindowMode::FullscreenExclusive ? " exclusive" : "",
    sConfig.mWindow.mMode != WindowMode::Windowed ? "fullscreen" : "windowed",
    sConfig.mWindow.mResizeable ? " resizeable" : ""
  );

  platform::Startup(sConfig.mWindow);
  input::Init();

  InitDearImGui();

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


void NewDearImGuiFrame() {
  auto& io = ImGui::GetIO();
  const auto windowSize = platform::GetWindowDesc().mSize;
  io.DisplaySize = ImVec2(
    static_cast<float>(windowSize.GetX()), static_cast<float>(windowSize.GetY())
  );
  io.DeltaTime = static_cast<float>(sCurrentDeltaTime);
  io.KeyCtrl = input::IsKeyPressed(input::Key::Control);
  io.KeyShift = input::IsKeyPressed(input::Key::Shift);
  io.KeyAlt = input::IsKeyPressed(input::Key::Alt);
  io.KeySuper = input::IsKeyPressed(input::Key::Super);
  io.MousePos = ImVec2(static_cast<float>(input::GetMousePos().GetX()), static_cast<float>(input::GetMousePos().GetY()));
  io.MouseDown[0] = input::IsMouseButtonPressed(input::MouseButton::Left);
  io.MouseDown[1] = input::IsMouseButtonPressed(input::MouseButton::Right);
  io.MouseDown[2] = input::IsMouseButtonPressed(input::MouseButton::Middle);
  io.MouseDown[3] = input::IsMouseButtonPressed(input::MouseButton::Button4);
  io.MouseDown[4] = input::IsMouseButtonPressed(input::MouseButton::Button5);
  gfx::GetRenderer()->NewGuiFrame();
  ImGui::NewFrame();
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

    NewDearImGuiFrame();

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
