#pragma once

#include <basalt/api/types.h>

#include <bitset>

namespace basalt {

struct Layer {
  Layer(const Layer&) = delete;
  Layer(Layer&&) noexcept = default;

  virtual ~Layer() = default;

  auto operator=(const Layer&) -> Layer& = delete;
  auto operator=(Layer&&) noexcept -> Layer& = default;

  [[nodiscard]] auto pointer_position() const noexcept -> PointerPosition;
  [[nodiscard]] auto is_mouse_button_down(MouseButton) const -> bool;
  [[nodiscard]] auto is_key_down(Key) const -> bool;

  [[nodiscard]] auto handle_input(const InputEvent&) -> bool;

  virtual void tick(Engine&) = 0;

protected:
  Layer() noexcept = default;

  virtual auto do_handle_input(const InputEvent&) -> InputEventHandled = 0;

private:
  PointerPosition mPointerPos;
  std::bitset<MOUSE_BUTTON_COUNT> mMouseButtonsDown;
  std::bitset<KEY_COUNT> mKeysDown;

  void update(const InputEvent&);
};

} // namespace basalt
