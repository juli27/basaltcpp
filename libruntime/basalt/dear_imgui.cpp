#include <basalt/dear_imgui.h>

#include <basalt/api/engine.h>
#include <basalt/api/input_events.h>

#include <basalt/gfx/backend/ext/dear_imgui_renderer.h>

#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/shared/size2d.h>

#include <basalt/api/base/enum_array.h>
#include <basalt/api/base/utils.h>

#include <imgui/imgui.h>

#include <algorithm>
#include <array>
#include <memory>
#include <utility>

namespace basalt {

using std::array;
using std::shared_ptr;

using gfx::CommandList;
using gfx::ext::DearImGuiCommandEncoder;
using gfx::ext::DearImGuiRenderer;

namespace {

// TODO: left <-> right variants
constexpr auto KEY_TO_IMGUI_KEY = EnumArray<Key, ImGuiKey, 102>{
  {Key::Unknown, ImGuiKey_None},
  {Key::F1, ImGuiKey_F1},
  {Key::F2, ImGuiKey_F2},
  {Key::F3, ImGuiKey_F3},
  {Key::F4, ImGuiKey_F4},
  {Key::F5, ImGuiKey_F5},
  {Key::F6, ImGuiKey_F6},
  {Key::F7, ImGuiKey_F7},
  {Key::F8, ImGuiKey_F8},
  {Key::F9, ImGuiKey_F9},
  {Key::F10, ImGuiKey_F10},
  {Key::F11, ImGuiKey_F11},
  {Key::F12, ImGuiKey_F12},
  {Key::Escape, ImGuiKey_Escape},
  {Key::Tab, ImGuiKey_Tab},
  {Key::CapsLock, ImGuiKey_CapsLock},
  {Key::Shift, ImGuiKey_LeftShift},
  {Key::Control, ImGuiKey_LeftCtrl},
  {Key::Alt, ImGuiKey_LeftAlt},
  {Key::Insert, ImGuiKey_Insert},
  {Key::Delete, ImGuiKey_Delete},
  {Key::Home, ImGuiKey_Home},
  {Key::End, ImGuiKey_End},
  {Key::PageUp, ImGuiKey_PageUp},
  {Key::PageDown, ImGuiKey_PageDown},
  {Key::Pause, ImGuiKey_Pause},
  {Key::LeftArrow, ImGuiKey_LeftArrow},
  {Key::RightArrow, ImGuiKey_RightArrow},
  {Key::UpArrow, ImGuiKey_UpArrow},
  {Key::DownArrow, ImGuiKey_DownArrow},
  {Key::Numpad0, ImGuiKey_Keypad0},
  {Key::Numpad1, ImGuiKey_Keypad1},
  {Key::Numpad2, ImGuiKey_Keypad2},
  {Key::Numpad3, ImGuiKey_Keypad3},
  {Key::Numpad4, ImGuiKey_Keypad4},
  {Key::Numpad5, ImGuiKey_Keypad5},
  {Key::Numpad6, ImGuiKey_Keypad6},
  {Key::Numpad7, ImGuiKey_Keypad7},
  {Key::Numpad8, ImGuiKey_Keypad8},
  {Key::Numpad9, ImGuiKey_Keypad9},
  {Key::NumpadAdd, ImGuiKey_KeypadAdd},
  {Key::NumpadSub, ImGuiKey_KeypadSubtract},
  {Key::NumpadMul, ImGuiKey_KeypadMultiply},
  {Key::NumpadDiv, ImGuiKey_KeypadDivide},
  {Key::NumpadDecimal, ImGuiKey_KeypadDecimal},
  {Key::NumpadLock, ImGuiKey_NumLock},
  {Key::NumpadEnter, ImGuiKey_KeypadEnter},
  {Key::Zero, ImGuiKey_0},
  {Key::One, ImGuiKey_1},
  {Key::Two, ImGuiKey_2},
  {Key::Three, ImGuiKey_3},
  {Key::Four, ImGuiKey_4},
  {Key::Five, ImGuiKey_5},
  {Key::Six, ImGuiKey_6},
  {Key::Seven, ImGuiKey_7},
  {Key::Eight, ImGuiKey_8},
  {Key::Nine, ImGuiKey_9},
  {Key::Backspace, ImGuiKey_Backspace},
  {Key::Space, ImGuiKey_Space},
  {Key::Enter, ImGuiKey_Enter},
  {Key::Menu, ImGuiKey_Menu},
  {Key::ScrollLock, ImGuiKey_ScrollLock},
  {Key::Print, ImGuiKey_PrintScreen},
  {Key::Plus, ImGuiKey_Equal},
  {Key::A, ImGuiKey_A},
  {Key::B, ImGuiKey_B},
  {Key::C, ImGuiKey_C},
  {Key::D, ImGuiKey_D},
  {Key::E, ImGuiKey_E},
  {Key::F, ImGuiKey_F},
  {Key::G, ImGuiKey_G},
  {Key::H, ImGuiKey_H},
  {Key::I, ImGuiKey_I},
  {Key::J, ImGuiKey_J},
  {Key::K, ImGuiKey_K},
  {Key::L, ImGuiKey_L},
  {Key::M, ImGuiKey_M},
  {Key::N, ImGuiKey_N},
  {Key::O, ImGuiKey_O},
  {Key::P, ImGuiKey_P},
  {Key::Q, ImGuiKey_Q},
  {Key::R, ImGuiKey_R},
  {Key::S, ImGuiKey_S},
  {Key::T, ImGuiKey_T},
  {Key::U, ImGuiKey_U},
  {Key::V, ImGuiKey_V},
  {Key::W, ImGuiKey_W},
  {Key::X, ImGuiKey_X},
  {Key::Y, ImGuiKey_Y},
  {Key::Z, ImGuiKey_Z},
  {Key::Minus, ImGuiKey_Minus},
  {Key::Comma, ImGuiKey_Comma},
  {Key::Period, ImGuiKey_Period},
  {Key::Oem1, ImGuiKey_Semicolon},
  {Key::Oem2, ImGuiKey_Slash},
  {Key::Oem3, ImGuiKey_GraveAccent},
  {Key::Oem4, ImGuiKey_LeftBracket},
  {Key::Oem5, ImGuiKey_Backslash},
  {Key::Oem6, ImGuiKey_RightBracket},
  {Key::Oem7, ImGuiKey_Apostrophe},
  {Key::Oem8, ImGuiKey_None},
  {Key::Oem9, ImGuiKey_None},
};
static_assert(KEY_TO_IMGUI_KEY.size() == KEY_COUNT);

constexpr auto to_imgui_key(Key const key) -> ImGuiKey {
  return KEY_TO_IMGUI_KEY[key];
}

} // namespace

auto DearImGui::create(gfx::Context& gfxContext) -> DearImGuiPtr {
  IMGUI_CHECKVERSION();

  auto* const ctx = ImGui::CreateContext();
  ImGui::SetCurrentContext(ctx);

  auto renderer =
    gfxContext.query_device_extension<DearImGuiRenderer>().value_or(nullptr);

  return std::make_shared<DearImGui>(std::move(renderer));
}

DearImGui::DearImGui(shared_ptr<DearImGuiRenderer> renderer)
  : mRenderer{std::move(renderer)} {
  auto& io = ImGui::GetIO();

  io.BackendPlatformName = "Basalt";
  io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;

  io.ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard;

  io.Fonts->AddFontDefault();
  io.Fonts->Build();

  if (mRenderer) {
    // this also uploads the font texture
    mRenderer->init();
  }
}

DearImGui::~DearImGui() noexcept {
  if (mRenderer) {
    mRenderer->shutdown();
  }

  ImGui::DestroyContext();
}

auto DearImGui::new_frame(UpdateContext const& ctx) const -> void {
  static_assert(ImGuiMouseButton_COUNT == MOUSE_BUTTON_COUNT);
  static_assert(KEY_COUNT <= 512);

  if (mRenderer) {
    mRenderer->new_frame();
  }

  auto& io = ImGui::GetIO();

  auto const displaySize = ctx.drawCtx.viewport;
  io.DisplaySize.x = static_cast<float>(displaySize.width());
  io.DisplaySize.y = static_cast<float>(displaySize.height());
  io.DeltaTime = ctx.deltaTime.count();

  auto& engine = ctx.engine;

  static_assert(ImGuiMouseCursor_COUNT == 9);
  if (!(io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)) {
    auto const cursor = ImGui::GetMouseCursor();
    // TODO: no mouse cursor / imgui cursor drawing
    if (cursor != ImGuiMouseCursor_None) {
      auto const mouseCursor = MouseCursor{static_cast<u8>(cursor)};
      if (mouseCursor != engine.mouse_cursor()) {
        engine.set_mouse_cursor(mouseCursor);
      }
    }
  }

  ImGui::NewFrame();
}

auto DearImGui::on_update(UpdateContext& ctx) -> void {
  // Context doesn't support builtin imgui rendering. Don't bother recording the
  // command
  if (!mRenderer) {
    return;
  }

  auto commandList = CommandList{};
  DearImGuiCommandEncoder::render_dear_imgui(commandList);

  ctx.drawCtx.commandLists.push_back(std::move(commandList));
}

auto DearImGui::on_input(InputEvent const& e) -> InputEventHandled {
  auto& io = ImGui::GetIO();

  switch (e.type) {
  case InputEventType::MouseMoved: {
    auto const& pointerPos = e.as<MouseMoved>().position;
    io.AddMousePosEvent(static_cast<float>(pointerPos.x()),
                        static_cast<float>(pointerPos.y()));

    return io.WantCaptureMouse ? InputEventHandled::Yes : InputEventHandled::No;
  }

  case InputEventType::MouseWheel:
    io.AddMouseWheelEvent(0.0f, e.as<MouseWheel>().offset);

    return io.WantCaptureMouse ? InputEventHandled::Yes : InputEventHandled::No;

  case InputEventType::MouseButtonDown:
    io.AddMouseButtonEvent(enum_cast(e.as<MouseButtonDown>().button), true);

    return io.WantCaptureMouse ? InputEventHandled::Yes : InputEventHandled::No;

  case InputEventType::MouseButtonUp:
    io.AddMouseButtonEvent(enum_cast(e.as<MouseButtonUp>().button), false);

    return io.WantCaptureMouse ? InputEventHandled::Yes : InputEventHandled::No;

  case InputEventType::KeyboardFocusGained:
  case InputEventType::KeyboardFocusLost:
    io.AddFocusEvent(e.type == InputEventType::KeyboardFocusGained);

    return io.WantCaptureKeyboard ? InputEventHandled::Yes
                                  : InputEventHandled::No;

  case InputEventType::KeyDown: {
    auto const key = e.as<KeyDown>().key;
    auto const imguiKey = to_imgui_key(key);

    if (key == Key::Shift) {
      io.AddKeyEvent(ImGuiMod_Shift, true);
    } else if (key == Key::Control) {
      io.AddKeyEvent(ImGuiMod_Ctrl, true);
    } else if (key == Key::Alt) {
      io.AddKeyEvent(ImGuiMod_Alt, true);
    }

    io.AddKeyEvent(imguiKey, true);
    io.SetKeyEventNativeData(imguiKey, enum_cast(key), 0);

    return io.WantCaptureKeyboard ? InputEventHandled::Yes
                                  : InputEventHandled::No;
  }

  case InputEventType::KeyUp: {
    auto const key = e.as<KeyUp>().key;
    auto const imguiKey = to_imgui_key(key);

    if (key == Key::Shift) {
      io.AddKeyEvent(ImGuiMod_Shift, false);
    } else if (key == Key::Control) {
      io.AddKeyEvent(ImGuiMod_Ctrl, false);
    } else if (key == Key::Alt) {
      io.AddKeyEvent(ImGuiMod_Alt, false);
    }

    io.AddKeyEvent(imguiKey, false);
    io.SetKeyEventNativeData(imguiKey, enum_cast(key), 0);

    return io.WantCaptureKeyboard ? InputEventHandled::Yes
                                  : InputEventHandled::No;
  }

  case InputEventType::CharacterTyped: {
    auto const& charTyped = e.as<CharacterTyped>().character;
    auto nullTerminatedChar = array<char, 5>{};
    std::copy(charTyped.begin(), charTyped.end(), nullTerminatedChar.begin());
    io.AddInputCharactersUTF8(nullTerminatedChar.data());

    return io.WantTextInput ? InputEventHandled::Yes : InputEventHandled::No;
  }
  }

  return InputEventHandled::No;
}

} // namespace basalt
