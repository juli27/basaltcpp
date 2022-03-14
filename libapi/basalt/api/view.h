#pragma once

#include <basalt/api/types.h>

#include <basalt/api/gfx/drawable.h>

#include <bitset>
#include <vector>

namespace basalt {

struct View : gfx::Drawable {
  View(const View&) = delete;
  View(View&&) noexcept = default;

  ~View() noexcept override = default;

  auto operator=(const View&) -> View& = delete;
  auto operator=(View&&) noexcept -> View& = default;

  [[nodiscard]] auto pointer_position() const noexcept -> PointerPosition;
  [[nodiscard]] auto is_mouse_button_down(MouseButton) const -> bool;
  [[nodiscard]] auto is_key_down(Key) const -> bool;

  void add_child_top(ViewPtr);
  void add_child_bottom(ViewPtr);
  void add_child_above(ViewPtr, const ViewPtr& before);
  void add_child_below(ViewPtr, const ViewPtr& after);
  void remove_child(const ViewPtr&);

  [[nodiscard]] auto handle_input(const InputEvent&) -> bool;

  void tick(Engine&);

protected:
  View() noexcept = default;

  virtual void on_tick(Engine&) {
  }

  virtual auto on_input(const InputEvent&) -> InputEventHandled {
    return InputEventHandled::No;
  }

private:
  // top to bottom order
  std::vector<ViewPtr> mViews;

  PointerPosition mPointerPos;
  std::bitset<MOUSE_BUTTON_COUNT> mMouseButtonsDown;
  std::bitset<KEY_COUNT> mKeysDown;

  void update(const InputEvent&);
};

} // namespace basalt
