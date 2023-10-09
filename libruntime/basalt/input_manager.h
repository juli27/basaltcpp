#pragma once

#include <basalt/api/input_events.h>
#include <basalt/api/types.h>

#include <basalt/api/base/types.h>

#include <array>
#include <bitset>
#include <vector>

namespace basalt {

// TODO: mouse leave
// TODO: mouse capturing
struct InputManager final {
  auto set_overlay(ViewPtr) -> void;

  auto dispatch_pending(ViewPtr const& root) -> void;

  auto mouse_moved(PointerPosition) -> void;

  auto mouse_wheel(f32 offset) -> void;

  auto mouse_button_down(MouseButton) -> void;
  auto mouse_button_up(MouseButton) -> void;

  auto keyboard_focus_gained() -> void;
  auto keyboard_focus_lost() -> void;
  auto key_down(Key) -> void;
  auto key_up(Key) -> void;

  auto character_utf8(std::array<char, 4>) -> void;

private:
  ViewPtr mOverlay;
  std::vector<InputEventPtr> mEvents;
  PointerPosition mPointerPos;
  std::bitset<MOUSE_BUTTON_COUNT> mMouseButtonsDown;
  std::bitset<KEY_COUNT> mKeysDown;
  bool hasKeyboardFocus{false};
};

} // namespace basalt
