#pragma once

#include "types.h"

#include <bitset>

namespace basalt {

struct InputLayer {
  InputLayer() = default;

  InputLayer(const InputLayer&) = delete;
  InputLayer(InputLayer&&) = default;

  virtual ~InputLayer() = default;

  auto operator=(const InputLayer&) -> InputLayer& = delete;
  auto operator=(InputLayer &&) -> InputLayer& = delete;

  [[nodiscard]] auto pointer_position() const noexcept -> PointerPosition;
  [[nodiscard]] auto is_mouse_button_down(MouseButton) const -> bool;
  [[nodiscard]] auto is_key_down(Key) const -> bool;

  [[nodiscard]] auto handle_input(const InputEvent&) -> bool;

protected:
  virtual auto do_handle_input(const InputEvent&) -> InputEventHandled = 0;

private:
  PointerPosition mPointerPos;
  std::bitset<MOUSE_BUTTON_COUNT> mMouseButtonsDown;
  std::bitset<KEY_COUNT> mKeysDown;

  void update(const InputEvent&);
};

} // namespace basalt
