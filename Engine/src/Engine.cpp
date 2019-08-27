#include "pch.h"

#include <basalt/Engine.h>

#include <chrono>
#include <memory>
#include <stdexcept>

#include <imgui/imgui.h> // ImGui

#include <basalt/Config.h>
#include <basalt/IApplication.h>
#include <basalt/Input.h>
#include <basalt/Log.h>
#include <basalt/Scene.h>
#include <basalt/common/Asserts.h>
#include <basalt/common/Types.h>
#include <basalt/gfx/Gfx.h>
#include <basalt/gfx/backend/IRenderer.h>
#include <basalt/platform/IGfxContext.h>
#include <basalt/platform/Platform.h> // platform
#include <basalt/platform/events/Event.h>
#include <basalt/platform/events/KeyEvents.h>

namespace basalt {

using ::std::shared_ptr;

using input::Key;
using input::MouseButton;
using platform::CharactersTyped;
using platform::Event;
using platform::EventDispatcher;
using platform::IGfxContext;
using platform::KeyPressedEvent;
using platform::KeyReleasedEvent;
using platform::MouseWheelScrolledEvent;

namespace {

Config sConfig{
  {"Basalt App", {1280, 720}, WindowMode::Fullscreen, false},
  {gfx::BackendApi::Default}
};
IApplication* sApp = nullptr;
f64 sCurrentDeltaTime = 0.0;
shared_ptr<Scene> sCurrentScene;
gfx::backend::IRenderer* sRenderer;

void InitDearImGui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  auto& imguiIo = ImGui::GetIO();
  imguiIo.ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard;
  imguiIo.KeyMap[ImGuiKey_Tab] = static_cast<i8>(Key::Tab);
  imguiIo.KeyMap[ImGuiKey_LeftArrow] = static_cast<i8>(Key::LeftArrow);
  imguiIo.KeyMap[ImGuiKey_RightArrow] = static_cast<i8>(Key::RightArrow);
  imguiIo.KeyMap[ImGuiKey_UpArrow] = static_cast<i8>(Key::UpArrow);
  imguiIo.KeyMap[ImGuiKey_DownArrow] = static_cast<i8>(Key::DownArrow);
  imguiIo.KeyMap[ImGuiKey_PageUp] = static_cast<i8>(Key::PageUp);
  imguiIo.KeyMap[ImGuiKey_PageDown] = static_cast<i8>(Key::PageDown);
  imguiIo.KeyMap[ImGuiKey_Home] = static_cast<i8>(Key::Home);
  imguiIo.KeyMap[ImGuiKey_End] = static_cast<i8>(Key::End);
  imguiIo.KeyMap[ImGuiKey_Insert] = static_cast<i8>(Key::Insert);
  imguiIo.KeyMap[ImGuiKey_Delete] = static_cast<i8>(Key::Delete);
  imguiIo.KeyMap[ImGuiKey_Backspace] = static_cast<i8>(Key::Backspace);
  imguiIo.KeyMap[ImGuiKey_Space] = static_cast<i8>(Key::Space);
  imguiIo.KeyMap[ImGuiKey_Enter] = static_cast<i8>(Key::Enter);
  imguiIo.KeyMap[ImGuiKey_Escape] = static_cast<i8>(Key::Escape);
  imguiIo.KeyMap[ImGuiKey_KeyPadEnter] = static_cast<i8>(Key::NumpadEnter);
  imguiIo.KeyMap[ImGuiKey_A] = static_cast<i8>(Key::A);
  imguiIo.KeyMap[ImGuiKey_C] = static_cast<i8>(Key::C);
  imguiIo.KeyMap[ImGuiKey_V] = static_cast<i8>(Key::V);
  imguiIo.KeyMap[ImGuiKey_X] = static_cast<i8>(Key::X);
  imguiIo.KeyMap[ImGuiKey_Y] = static_cast<i8>(Key::Y);
  imguiIo.KeyMap[ImGuiKey_Z] = static_cast<i8>(Key::Z);

  platform::AddEventListener([](const Event& e) {
    const EventDispatcher dispatcher(e);
    auto& io = ImGui::GetIO();
    dispatcher.Dispatch<KeyPressedEvent>([&](const KeyPressedEvent& event) {
      io.KeysDown[static_cast<i8>(event.mKey)] = true;
    });
    dispatcher.Dispatch<KeyReleasedEvent>([&](const KeyReleasedEvent& event) {
      io.KeysDown[static_cast<i8>(event.mKey)] = false;
    });
    dispatcher.Dispatch<CharactersTyped>([&](const CharactersTyped& event) {
      io.AddInputCharactersUTF8(event.mChars.c_str());
    });
    dispatcher.Dispatch<MouseWheelScrolledEvent>(
      [&](const MouseWheelScrolledEvent& event) {
      io.MouseWheel = event.mOffset;
    });
  });
}

void Startup() {
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

  platform::startup(sConfig);
  input::Init();

  // init imgui before gfx. Renderer initializes imgui render backend
  InitDearImGui();
  sRenderer =  platform::get_window_data().mGfxContext->create_renderer();

  BS_INFO("engine startup complete");
}


void Shutdown() {
  BS_INFO("shutting down...");

  delete sRenderer;
  sRenderer = nullptr;

  ImGui::DestroyContext();

  platform::Shutdown();

  delete sApp;
  sApp = nullptr;

  BS_INFO("engine shutdown");
}


void NewDearImGuiFrame() {
  auto& io = ImGui::GetIO();
  const auto windowSize = platform::get_window_data().mSize;
  io.DisplaySize = ImVec2(
    static_cast<float>(windowSize.GetX()), static_cast<float>(windowSize.GetY())
  );
  io.DeltaTime = static_cast<float>(sCurrentDeltaTime);
  io.KeyCtrl = input::IsKeyPressed(Key::Control);
  io.KeyShift = input::IsKeyPressed(Key::Shift);
  io.KeyAlt = input::IsKeyPressed(Key::Alt);
  io.KeySuper = input::IsKeyPressed(Key::Super);
  io.MousePos = ImVec2(static_cast<float>(input::GetMousePos().GetX()), static_cast<float>(input::GetMousePos().GetY()));
  io.MouseDown[0] = input::IsMouseButtonPressed(MouseButton::Left);
  io.MouseDown[1] = input::IsMouseButtonPressed(MouseButton::Right);
  io.MouseDown[2] = input::IsMouseButtonPressed(MouseButton::Middle);
  io.MouseDown[3] = input::IsMouseButtonPressed(MouseButton::Button4);
  io.MouseDown[4] = input::IsMouseButtonPressed(MouseButton::Button5);
  sRenderer->NewGuiFrame();
  ImGui::NewFrame();
}

} // namespace


void Run() {
  Startup();

  sApp->OnInit();
  BS_ASSERT(sCurrentScene, "no scene set");

  BS_INFO("entering main loop");

  static_assert(std::chrono::high_resolution_clock::is_steady);
  using Clock = std::chrono::high_resolution_clock;

  auto startTime = Clock::now();
  while (platform::PollEvents()) {
    NewDearImGuiFrame();

    sApp->OnUpdate();

    // TODO: use the asynchronicity of the graphics API runtime and gpu driver
    // also calls ImGui::Render()
    gfx::render(sRenderer, sCurrentScene);
    platform::get_window_data().mGfxContext->present();

    const auto endTime = Clock::now();
    sCurrentDeltaTime = static_cast<f64>((endTime - startTime).count()) /
      (Clock::period::den * Clock::period::num);
    startTime = endTime;
  }

  BS_INFO("leaving main loop");

  sApp->OnShutdown();

  Shutdown();
}


auto GetDeltaTime() -> f64 {
  return sCurrentDeltaTime;
}


void SetCurrentScene(const shared_ptr<Scene>& scene) {
  sCurrentScene = scene;
}

auto get_renderer() -> gfx::backend::IRenderer* {
  return sRenderer;
}

} // namespace basalt
