#include <basalt/dear_imgui.h>

#include <basalt/api/engine.h>
#include <basalt/api/input_events.h>

#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/utils.h>
#include <basalt/api/gfx/backend/ext/dear_imgui_renderer.h>

#include <basalt/api/shared/size2d.h>

#include <basalt/api/base/utils.h>

#include <imgui/imgui.h>

#include <algorithm>
#include <array>
#include <utility>

namespace basalt {

using gfx::CommandList;
using gfx::Device;
using gfx::ResourceCache;
using gfx::ext::CommandRenderDearImGui;
using gfx::ext::DearImGuiRenderer;

DearImGui::DearImGui(Device& gfxDevice) {
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

  mRenderer = gfx::query_device_extension<DearImGuiRenderer>(gfxDevice).value();
  mRenderer->init();
}

DearImGui::~DearImGui() {
  mRenderer->shutdown();

  ImGui::DestroyContext();
}

auto DearImGui::draw(ResourceCache&, Size2Du16, const RectangleU16&)
  -> std::tuple<CommandList, RectangleU16> {
  CommandList commandList {};
  commandList.add<CommandRenderDearImGui>();

  return {std::move(commandList), RectangleU16 {}};
}

void DearImGui::tick(Engine& engine) {
  static_assert(ImGuiMouseButton_COUNT == MOUSE_BUTTON_COUNT);
  static_assert(KEY_COUNT <= 512);

  mRenderer->new_frame();

  ImGuiIO& io {ImGui::GetIO()};

  const Size2Du16 displaySize {engine.window_surface_size()};
  io.DisplaySize.x = static_cast<float>(displaySize.width());
  io.DisplaySize.y = static_cast<float>(displaySize.height());
  io.DeltaTime = static_cast<float>(engine.delta_time());
  io.KeyCtrl = is_key_down(Key::Control);
  io.KeyShift = is_key_down(Key::Shift);
  io.KeyAlt = is_key_down(Key::Alt);

  // TODO: reenable once super/meta key has been implemented on linux/macOS
  //       the super key mapping to the windows key on windows caused
  //       some interoperability problems with OS functionality e.g.
  //       a pressed down super key sticking around after Win+V
  // io.KeySuper = false;

  static_assert(ImGuiMouseCursor_COUNT == 9);
  if (!(io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)) {
    const ImGuiMouseCursor cursor = ImGui::GetMouseCursor();
    // TODO: no mouse cursor / imgui cursor drawing
    if (cursor != ImGuiMouseCursor_None) {
      const auto mouseCursor {MouseCursor {static_cast<u8>(cursor)}};
      if (mouseCursor != engine.mouse_cursor()) {
        engine.set_mouse_cursor(mouseCursor);
      }
    }
  }

  ImGui::NewFrame();
}

auto DearImGui::do_handle_input(const InputEvent& e) -> InputEventHandled {
  ImGuiIO& io {ImGui::GetIO()};

  switch (e.type) {
  case InputEventType::MouseMoved: {
    const PointerPosition& pointerPos {e.as<MouseMoved>().position};
    io.MousePos.x = static_cast<float>(pointerPos.x());
    io.MousePos.y = static_cast<float>(pointerPos.y());

    return io.WantCaptureMouse ? InputEventHandled::Yes : InputEventHandled::No;
  }

  case InputEventType::MouseWheel:
    io.MouseWheel += e.as<MouseWheel>().offset;

    break;

  case InputEventType::MouseButtonDown:
    io.MouseDown[enum_cast(e.as<MouseButtonDown>().button)] = true;

    break;

  case InputEventType::MouseButtonUp:
    io.MouseDown[enum_cast(e.as<MouseButtonUp>().button)] = false;

    break;

  case InputEventType::KeyDown:
    io.KeysDown[enum_cast(e.as<KeyDown>().key)] = true;

    return io.WantCaptureKeyboard ? InputEventHandled::Yes
                                  : InputEventHandled::No;

  case InputEventType::KeyUp:
    io.KeysDown[enum_cast(e.as<KeyUp>().key)] = false;

    return io.WantCaptureKeyboard ? InputEventHandled::Yes
                                  : InputEventHandled::No;

  case InputEventType::CharacterTyped: {
    const auto charTyped = e.as<CharacterTyped>().character;
    std::array<char, 5> nullTerminatedChar {};
    std::copy(charTyped.begin(), charTyped.end(), nullTerminatedChar.begin());
    io.AddInputCharactersUTF8(nullTerminatedChar.data());

    return io.WantTextInput ? InputEventHandled::Yes : InputEventHandled::No;
  }

  default:
    break;
  }

  return InputEventHandled::No;
}

} // namespace basalt
