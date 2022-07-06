#include <basalt/dear_imgui.h>

#include <basalt/api/engine.h>
#include <basalt/api/input_events.h>

#include <basalt/gfx/backend/utils.h>

#include <basalt/api/gfx/backend/command_list.h>
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
using gfx::ext::DearImGuiRenderer;

auto DearImGui::create(Device& gfxDevice, void* const imeWindowHandle)
  -> DearImGuiPtr {
  IMGUI_CHECKVERSION();

  auto* const ctx {ImGui::CreateContext()};
  ImGui::SetCurrentContext(ctx);

  auto renderer {
    gfx::query_device_extension<DearImGuiRenderer>(gfxDevice).value_or(
      nullptr)};

  return std::make_shared<DearImGui>(std::move(renderer), imeWindowHandle);
}

DearImGui::~DearImGui() noexcept {
  if (mRenderer) {
    mRenderer->shutdown();
  }

  ImGui::DestroyContext();
}

auto DearImGui::new_frame(Engine& engine, const Size2Du16 displaySize) const
  -> void {
  static_assert(ImGuiMouseButton_COUNT == MOUSE_BUTTON_COUNT);
  static_assert(KEY_COUNT <= 512);

  if (mRenderer) {
    mRenderer->new_frame();
  }

  ImGuiIO& io {ImGui::GetIO()};

  io.DisplaySize.x = static_cast<float>(displaySize.width());
  io.DisplaySize.y = static_cast<float>(displaySize.height());
  io.DeltaTime = static_cast<float>(engine.delta_time());
  io.KeyCtrl = is_key_down(Key::Control);
  io.KeyShift = is_key_down(Key::Shift);
  io.KeyAlt = is_key_down(Key::Alt);
  io.KeySuper = false;

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

DearImGui::DearImGui(std::shared_ptr<DearImGuiRenderer> renderer,
                     void* const imeWindowHandle)
  : mRenderer {std::move(renderer)} {
  auto& io {ImGui::GetIO()};

  io.BackendPlatformName = "Basalt";
  io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
  io.ImeWindowHandle = imeWindowHandle;

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

  io.ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard;

  io.Fonts->AddFontDefault();
  io.Fonts->Build();

  if (mRenderer) {
    // this also uploads the font texture
    mRenderer->init();
  }
}

auto DearImGui::on_draw(const DrawContext& context) -> void {
  CommandList commandList {};
  commandList.ext_render_dear_imgui();

  context.commandLists.push_back(std::move(commandList));
}

auto DearImGui::on_input(const InputEvent& e) -> InputEventHandled {
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

    return io.WantCaptureMouse ? InputEventHandled::Yes : InputEventHandled::No;

  case InputEventType::MouseButtonDown:
    io.MouseDown[enum_cast(e.as<MouseButtonDown>().button)] = true;

    return io.WantCaptureMouse ? InputEventHandled::Yes : InputEventHandled::No;

  case InputEventType::MouseButtonUp:
    io.MouseDown[enum_cast(e.as<MouseButtonUp>().button)] = false;

    return io.WantCaptureMouse ? InputEventHandled::Yes : InputEventHandled::No;

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
  }

  return InputEventHandled::No;
}

} // namespace basalt
