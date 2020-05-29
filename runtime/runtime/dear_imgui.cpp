#include "runtime/dear_imgui.h"

#include "runtime/Input.h"

#include "runtime/gfx/backend/IRenderer.h"
#include "runtime/platform/Platform.h"

#include "runtime/platform/events/Event.h"
#include "runtime/platform/events/KeyEvents.h"

#include "runtime/math/Vec2.h"
#include "runtime/shared/Size2D.h"

#include <imgui/imgui.h>

namespace basalt {

using gfx::backend::IRenderer;
using platform::Event;
using platform::EventDispatcher;
using platform::Key;
using platform::KeyPressedEvent;
using platform::KeyReleasedEvent;
using math::Vec2i32;

DearImGui::DearImGui(IRenderer* const renderer)
  : mRenderer {renderer} {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  auto& io = ImGui::GetIO();
  io.BackendPlatformName = "Basalt";
  io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;

  io.KeyMap[ImGuiKey_Tab] = enum_cast(Key::Tab);
  io.KeyMap[ImGuiKey_LeftArrow] = enum_cast(Key::LeftArrow);
  io.KeyMap[ImGuiKey_RightArrow] = enum_cast(Key::RightArrow);
  io.KeyMap[ImGuiKey_UpArrow] = enum_cast(Key::UpArrow);
  io.KeyMap[ImGuiKey_DownArrow] = enum_cast(Key::DownArrow);
  io.KeyMap[ImGuiKey_PageUp] = enum_cast(Key::PageUp);
  io.KeyMap[ImGuiKey_PageDown] = enum_cast(Key::PageDown);
  io.KeyMap[ImGuiKey_Home] = enum_cast(Key::Home);
  io.KeyMap[ImGuiKey_End] = enum_cast(Key::End);
  io.KeyMap[ImGuiKey_Insert] = enum_cast(Key::Insert);
  io.KeyMap[ImGuiKey_Delete] = enum_cast(Key::Delete);
  io.KeyMap[ImGuiKey_Backspace] = enum_cast(Key::Backspace);
  io.KeyMap[ImGuiKey_Space] = enum_cast(Key::Space);
  io.KeyMap[ImGuiKey_Enter] = enum_cast(Key::Enter);
  io.KeyMap[ImGuiKey_Escape] = enum_cast(Key::Escape);
  io.KeyMap[ImGuiKey_KeyPadEnter] = enum_cast(Key::NumpadEnter);
  io.KeyMap[ImGuiKey_A] = enum_cast(Key::A);
  io.KeyMap[ImGuiKey_C] = enum_cast(Key::C);
  io.KeyMap[ImGuiKey_V] = enum_cast(Key::V);
  io.KeyMap[ImGuiKey_X] = enum_cast(Key::X);
  io.KeyMap[ImGuiKey_Y] = enum_cast(Key::Y);
  io.KeyMap[ImGuiKey_Z] = enum_cast(Key::Z);

  platform::add_event_listener(
    [](const Event& e) {
      auto& io = ImGui::GetIO();

      const EventDispatcher dispatcher {e};
      dispatcher.dispatch<KeyPressedEvent>(
        [&](const KeyPressedEvent& event) {
          io.KeysDown[enum_cast(event.key)] = true;
        });
      dispatcher.dispatch<KeyReleasedEvent>(
        [&](const KeyReleasedEvent& event) {
          io.KeysDown[enum_cast(event.key)] = false;
        });
    });

  static_assert(input::KEY_COUNT < 512);

  mRenderer->init_dear_imgui();
}

DearImGui::~DearImGui() {
  mRenderer->shutdown_dear_imgui();

  ImGui::DestroyContext();
}

void DearImGui::new_frame(const UpdateContext& ctx) const {
  mRenderer->new_gui_frame();

  auto& io = ImGui::GetIO();
  const Input& input {ctx.input};

  const Vec2i32 mousePos {input.mouse_position()};
  io.MousePos = ImVec2 {
    static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)
  };

  static_assert(ImGuiMouseButton_COUNT == MOUSE_BUTTON_COUNT);

  for (const InputEventPtr& event : input.events()) {
    switch (event->type) {
    case InputEventType::MouseButtonPressed: {
      const auto* mbPressed {event->as<MouseButtonPressed>()};
      io.MouseDown[enum_cast(mbPressed->button)] = true;
      break;
    }

    case InputEventType::MouseButtonReleased: {
      const auto* mbReleased {event->as<MouseButtonReleased>()};
      io.MouseDown[enum_cast(mbReleased->button)] = false;
      break;
    }

    case InputEventType::MouseWheel: {
      const auto* mouseWheel {event->as<MouseWheel>()};
      io.MouseWheel += mouseWheel->offset;
      break;
    }

    case InputEventType::CharactersTyped: {
      const auto* charactersTyped {event->as<CharactersTyped>()};
      io.AddInputCharactersUTF8(charactersTyped->chars.c_str());
    }

    default:
      break;
    }
  }

  io.DisplaySize = ImVec2(
    static_cast<float>(ctx.windowSize.width())
  , static_cast<float>(ctx.windowSize.height())
  );
  io.DeltaTime = static_cast<float>(ctx.deltaTime);
  io.KeyCtrl = input::is_key_pressed(Key::Control);
  io.KeyShift = input::is_key_pressed(Key::Shift);
  io.KeyAlt = input::is_key_pressed(Key::Alt);

  // TODO: reenable once super/meta key has been implemented on linux/osx
  //       the super key mapping to the windows key on windows caused
  //       some interoperability problems with OS functionality e.g.
  //       a pressed down super key sticking around after Win+V
  io.KeySuper = false;

  static_assert(ImGuiMouseCursor_COUNT == 9);
  if (!(io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)) {
    const ImGuiMouseCursor cursor = ImGui::GetMouseCursor();
    // TODO: no mouse cursor / imgui cursor drawing
    if (cursor != ImGuiMouseCursor_None) {
      ctx.engine.mouseCursor = MouseCursor {static_cast<u8>(cursor)};
    }
  }

  ImGui::NewFrame();
}

} // namespace basalt
