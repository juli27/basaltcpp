#pragma once

#include <basalt/api/types.h>

#include <bitset>

namespace basalt {

struct InputState final {
  PointerPosition pointerPos;
  std::bitset<MOUSE_BUTTON_COUNT> mouseButtonsDown;
  std::bitset<KEY_COUNT> keysDown;

  [[nodiscard]] auto is_mouse_button_down(MouseButton) const -> bool;
  [[nodiscard]] auto is_key_down(Key) const -> bool;

  auto update(const InputEvent&) -> void;
};

} // namespace basalt
