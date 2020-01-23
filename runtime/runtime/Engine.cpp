#include "runtime/Engine.h"

#include "runtime/IApplication.h"
#include "runtime/Input.h"
#include "runtime/Scene.h"

#include "runtime/gfx/Gfx.h"
#include "runtime/gfx/backend/IRenderer.h"

#include "runtime/platform/IGfxContext.h"
#include "runtime/platform/Platform.h"

#include "runtime/platform/events/Event.h"
#include "runtime/platform/events/KeyEvents.h"
#include "runtime/platform/events/MouseEvents.h"
#include "runtime/platform/events/WindowEvents.h"

#include "runtime/shared/Asserts.h"
#include "runtime/shared/Config.h"
#include "runtime/shared/Log.h"
#include "runtime/shared/Types.h"

#include <imgui/imgui.h>

#include <chrono>
#include <memory>
#include <stdexcept>

using std::shared_ptr;

namespace basalt {

using input::Key;
using input::MouseButton;
using platform::CharactersTyped;
using platform::Event;
using platform::EventDispatcher;
using platform::EventType;
using platform::KeyPressedEvent;
using platform::KeyReleasedEvent;
using platform::MouseWheelScrolledEvent;
using platform::WindowResizedEvent;

namespace {

Config sConfig{};
IApplication* sApp = nullptr;
f64 sCurrentDeltaTime{0.0};
shared_ptr<Scene> sCurrentScene;
gfx::backend::IRenderer* sRenderer;
bool sRunning = true;

void init_dear_imgui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  auto& imguiIo = ImGui::GetIO();
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

  platform::add_event_listener([](const Event& e) {
    const EventDispatcher dispatcher(e);
    auto& io = ImGui::GetIO();
    dispatcher.dispatch<KeyPressedEvent>([&](const KeyPressedEvent& event) {
      io.KeysDown[static_cast<i8>(event.mKey)] = true;
    });
    dispatcher.dispatch<KeyReleasedEvent>([&](const KeyReleasedEvent& event) {
      io.KeysDown[static_cast<i8>(event.mKey)] = false;
    });
    dispatcher.dispatch<CharactersTyped>([&](const CharactersTyped& event) {
      io.AddInputCharactersUTF8(event.mChars.c_str());
    });
    dispatcher.dispatch<MouseWheelScrolledEvent>(
      [&](const MouseWheelScrolledEvent& event) {
      io.MouseWheel = event.mOffset;
    });
  });
}

void startup() {
  BASALT_LOG_INFO("platform: {}", platform::get_name());

  // TODO: load config from file or create default

  sApp = IApplication::create();
  if (!sApp) {
    throw std::runtime_error("failed to create IApplication object");
  }

  BASALT_LOG_INFO("config");
  BASALT_LOG_INFO("  app name: {}", sConfig.mAppName);
  BASALT_LOG_INFO("  window: {}x{}{} {}{}",
    sConfig.mWindowWidth,
    sConfig.mWindowHeight,
    sConfig.mWindowMode == WindowMode::FullscreenExclusive ? " exclusive" : "",
    sConfig.mWindowMode != WindowMode::Windowed ? "fullscreen" : "windowed",
    sConfig.mIsWindowResizeable ? " resizeable" : ""
  );

  platform::startup(sConfig);
  input::init();

  // init imgui before gfx. Renderer initializes imgui render backend
  init_dear_imgui();
  sRenderer =  platform::get_window_gfx_context()->create_renderer();
}

void shutdown() {
  delete sRenderer;
  sRenderer = nullptr;

  ImGui::DestroyContext();

  platform::shutdown();

  delete sApp;
  sApp = nullptr;
}

void new_dear_im_gui_frame() {
  auto& io = ImGui::GetIO();
  const auto windowSize = platform::get_window_size();
  io.DisplaySize = ImVec2(
    static_cast<float>(windowSize.x()), static_cast<float>(windowSize.y())
  );
  io.DeltaTime = static_cast<float>(sCurrentDeltaTime);
  io.KeyCtrl = input::is_key_pressed(Key::Control);
  io.KeyShift = input::is_key_pressed(Key::Shift);
  io.KeyAlt = input::is_key_pressed(Key::Alt);
  io.KeySuper = input::is_key_pressed(Key::Super);
  io.MousePos = ImVec2(static_cast<float>(input::get_mouse_pos().x()), static_cast<float>(input::get_mouse_pos().y()));
  io.MouseDown[0] = input::is_mouse_button_pressed(MouseButton::Left);
  io.MouseDown[1] = input::is_mouse_button_pressed(MouseButton::Right);
  io.MouseDown[2] = input::is_mouse_button_pressed(MouseButton::Middle);
  io.MouseDown[3] = input::is_mouse_button_pressed(MouseButton::Button4);
  io.MouseDown[4] = input::is_mouse_button_pressed(MouseButton::Button5);
  sRenderer->new_gui_frame();
  ImGui::NewFrame();
}

void dispatch_pending_events() {
  const auto events = platform::poll_events();
  for (const auto& event : events) {
    switch (event->mType) {
    case EventType::Quit:
    case EventType::WindowCloseRequest:
      sRunning = false;
      break;

    case EventType::WindowResized: {
      const auto resizedEvent = std::static_pointer_cast<WindowResizedEvent>(event);
      sRenderer->on_window_resize(*resizedEvent);
      break;
    }

    default:
      break;
    }
  }
}

} // namespace

void run() {
  startup();

  sApp->on_init();
  BASALT_ASSERT(sCurrentScene, "no scene set");

  static_assert(std::chrono::high_resolution_clock::is_steady);
  using Clock = std::chrono::high_resolution_clock;

  auto startTime = Clock::now();
  do {
    new_dear_im_gui_frame();
    sApp->on_update();

    // also calls ImGui::Render()
    gfx::render(sRenderer, sCurrentScene);

    platform::get_window_gfx_context()->present();

    const auto endTime = Clock::now();
    sCurrentDeltaTime = static_cast<f64>((endTime - startTime).count()) /
      (Clock::period::den * Clock::period::num);
    startTime = endTime;

    dispatch_pending_events();
  } while (sRunning);

  sApp->on_shutdown();

  shutdown();
}

void quit() {
  sRunning = false;
}

auto get_delta_time() -> f64 {
  return sCurrentDeltaTime;
}

void set_current_scene(const shared_ptr<Scene>& scene) {
  sCurrentScene = scene;
}

auto get_renderer() -> gfx::backend::IRenderer* {
  return sRenderer;
}

} // namespace basalt
