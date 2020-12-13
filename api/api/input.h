#pragma once

#include "input_events.h"
#include "types.h"

#include "base/types.h"

#include <bitset>
#include <string>
#include <vector>

namespace basalt {

struct Input final {
  Input() = default;

  Input(const Input&) = delete;
  Input(Input&&) = default;

  ~Input() = default;

  auto operator=(const Input&) -> Input& = delete;
  auto operator=(Input &&) -> Input& = delete;

  [[nodiscard]] auto events() const noexcept
    -> const std::vector<InputEventPtr>&;

  [[nodiscard]] auto cursor_position() const noexcept -> CursorPosition;
  void mouse_moved(CursorPosition);

  void mouse_wheel(f32 offset);

  [[nodiscard]] auto is_mouse_button_down(MouseButton) const -> bool;
  void mouse_button_down(MouseButton);
  void mouse_button_up(MouseButton);

  [[nodiscard]] auto is_key_down(Key) const -> bool;
  void key_down(Key);
  void key_up(Key);

  void characters_typed(std::string);

private:
  std::vector<InputEventPtr> mEvents;
  std::bitset<MOUSE_BUTTON_COUNT> mMouseButtonsDown;
  CursorPosition mMousePosition;
  std::bitset<KEY_COUNT> mKeysDown;
};

} // namespace basalt
