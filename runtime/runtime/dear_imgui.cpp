#include "runtime/dear_imgui.h"

#include "runtime/Engine.h"
#include "runtime/Input.h"

#include "runtime/gfx/backend/IRenderer.h"
#include "runtime/platform/Platform.h"

#include "runtime/platform/events/Event.h"
#include "runtime/platform/events/KeyEvents.h"
#include "runtime/platform/events/MouseEvents.h"

#include <imgui/imgui.h>

namespace basalt {

using gfx::backend::IRenderer;
using platform::CharactersTyped;
using platform::Event;
using platform::EventDispatcher;
using platform::Key;
using platform::KeyPressedEvent;
using platform::KeyReleasedEvent;
using platform::MouseButton;
using platform::MouseWheelScrolledEvent;

void DearImGui::init() {
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

void DearImGui::shutdown() {
  ImGui::DestroyContext();
}

void DearImGui::new_frame(IRenderer* const renderer, const f64 deltaTime) {
  auto& io = ImGui::GetIO();
  const auto windowSize = platform::get_window_size();
  io.DisplaySize = ImVec2(
    static_cast<float>(windowSize.width()), static_cast<float>(windowSize.height())
  );
  io.DeltaTime = static_cast<float>(deltaTime);
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
  renderer->new_gui_frame();
  ImGui::NewFrame();
}

} // namespace basalt
