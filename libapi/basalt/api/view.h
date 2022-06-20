#pragma once

#include <basalt/api/types.h>

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/shared/size2d.h>
#include <basalt/api/shared/types.h>

#include <bitset>
#include <vector>

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

  auto add_child_top(ViewPtr) -> void;
  auto add_child_bottom(ViewPtr) -> void;
  auto add_child_above(ViewPtr, const ViewPtr& before) -> void;
  auto add_child_below(ViewPtr, const ViewPtr& after) -> void;
  auto remove_child(const ViewPtr&) -> void;

  struct DrawContext final {
    std::vector<gfx::CommandList>& commandLists;
    gfx::ResourceCache& cache;
    Size2Du16 viewport;
  };

  auto draw(const DrawContext& context) -> void;

  [[nodiscard]] auto handle_input(const InputEvent&) -> bool;

  auto tick(Engine&) -> void;

protected:
  View() noexcept = default;

  virtual auto on_draw(const DrawContext&) -> void;

  virtual auto on_tick(Engine&) -> void;

  [[nodiscard]] virtual auto on_input(const InputEvent&) -> InputEventHandled;

private:
  // top to bottom order
  std::vector<ViewPtr> mChildren;

  PointerPosition mPointerPos;
  std::bitset<MOUSE_BUTTON_COUNT> mMouseButtonsDown;
  std::bitset<KEY_COUNT> mKeysDown;

  auto update(const InputEvent&) -> void;
};

} // namespace basalt
