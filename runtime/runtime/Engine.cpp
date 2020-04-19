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
  imguiIo.KeyMap[ImGuiKey_Tab] = enum_cast(Key::Tab);
  imguiIo.KeyMap[ImGuiKey_LeftArrow] = enum_cast(Key::LeftArrow);
  imguiIo.KeyMap[ImGuiKey_RightArrow] = enum_cast(Key::RightArrow);
  imguiIo.KeyMap[ImGuiKey_UpArrow] = enum_cast(Key::UpArrow);
  imguiIo.KeyMap[ImGuiKey_DownArrow] = enum_cast(Key::DownArrow);
  imguiIo.KeyMap[ImGuiKey_PageUp] = enum_cast(Key::PageUp);
  imguiIo.KeyMap[ImGuiKey_PageDown] = enum_cast(Key::PageDown);
  imguiIo.KeyMap[ImGuiKey_Home] = enum_cast(Key::Home);
  imguiIo.KeyMap[ImGuiKey_End] = enum_cast(Key::End);
  imguiIo.KeyMap[ImGuiKey_Insert] = enum_cast(Key::Insert);
  imguiIo.KeyMap[ImGuiKey_Delete] = enum_cast(Key::Delete);
  imguiIo.KeyMap[ImGuiKey_Backspace] = enum_cast(Key::Backspace);
  imguiIo.KeyMap[ImGuiKey_Space] = enum_cast(Key::Space);
  imguiIo.KeyMap[ImGuiKey_Enter] = enum_cast(Key::Enter);
  imguiIo.KeyMap[ImGuiKey_Escape] = enum_cast(Key::Escape);
  imguiIo.KeyMap[ImGuiKey_KeyPadEnter] = enum_cast(Key::NumpadEnter);
  imguiIo.KeyMap[ImGuiKey_A] = enum_cast(Key::A);
  imguiIo.KeyMap[ImGuiKey_C] = enum_cast(Key::C);
  imguiIo.KeyMap[ImGuiKey_V] = enum_cast(Key::V);
  imguiIo.KeyMap[ImGuiKey_X] = enum_cast(Key::X);
  imguiIo.KeyMap[ImGuiKey_Y] = enum_cast(Key::Y);
  imguiIo.KeyMap[ImGuiKey_Z] = enum_cast(Key::Z);

  platform::add_event_listener([](const Event& e) {
    const EventDispatcher dispatcher(e);
    auto& io = ImGui::GetIO();
    dispatcher.dispatch<KeyPressedEvent>([&](const KeyPressedEvent& event) {
      io.KeysDown[enum_cast(event.mKey)] = true;
    });
    dispatcher.dispatch<KeyReleasedEvent>([&](const KeyReleasedEvent& event) {
      io.KeysDown[enum_cast(event.mKey)] = false;
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
  BASALT_LOG_INFO("starting on platform {}", platform::get_name());

  // TODO: load config from file or create default

  sApp = IApplication::create();
  if (!sApp) {
    throw std::runtime_error("failed to create IApplication object");
  }

  BASALT_LOG_INFO("config");
  BASALT_LOG_INFO("  app name: {}", sConfig.appName);
  BASALT_LOG_INFO("  window: {}x{}{} {}{}",
    sConfig.windowSize.width(),
    sConfig.windowSize.height(),
    sConfig.windowMode == WindowMode::FullscreenExclusive ? " exclusive" : "",
    sConfig.windowMode != WindowMode::Windowed ? "fullscreen" : "windowed",
    sConfig.isWindowResizeable ? " resizeable" : ""
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
    static_cast<float>(windowSize.width()), static_cast<float>(windowSize.height())
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
