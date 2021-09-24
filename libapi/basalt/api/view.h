#pragma once

#include <basalt/api/types.h>

#include <bitset>

namespace basalt {

struct View {
  View(const View&) = delete;
  View(View&&) noexcept = default;

  virtual ~View() noexcept = default;

  auto operator=(const View&) -> View& = delete;
  auto operator=(View&&) noexcept -> View& = default;

  [[nodiscard]] auto pointer_position() const noexcept -> PointerPosition;
  [[nodiscard]] auto is_mouse_button_down(MouseButton) const -> bool;
  [[nodiscard]] auto is_key_down(Key) const -> bool;

  [[nodiscard]] auto handle_input(const InputEvent&) -> bool;

  virtual void tick(Engine&) = 0;

protected:
  View() noexcept = default;

  virtual auto do_handle_input(const InputEvent&) -> InputEventHandled {
    return InputEventHandled::No;
  }

private:
  PointerPosition mPointerPos;
  std::bitset<MOUSE_BUTTON_COUNT> mMouseButtonsDown;
  std::bitset<KEY_COUNT> mKeysDown;

  void update(const InputEvent&);
};

} // namespace basalt
