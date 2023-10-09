#pragma once

#include <basalt/api/input.h>
#include <basalt/api/types.h>

#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/shared/size2d.h>
#include <basalt/api/shared/types.h>

#include <vector>

namespace basalt {

class View {
public:
  View(View const&) = delete;
  View(View&&) noexcept = default;

  virtual ~View() noexcept = default;

  auto operator=(View const&) -> View& = delete;
  auto operator=(View&&) noexcept -> View& = default;

  [[nodiscard]] auto input_state() const noexcept -> InputState const&;
  [[nodiscard]] auto pointer_position() const noexcept -> PointerPosition;
  [[nodiscard]] auto is_mouse_button_down(MouseButton) const -> bool;
  [[nodiscard]] auto is_key_down(Key) const -> bool;

  auto add_child_top(ViewPtr) -> void;
  auto add_child_bottom(ViewPtr) -> void;
  auto add_child_above(ViewPtr, ViewPtr const& before) -> void;
  auto add_child_below(ViewPtr, ViewPtr const& after) -> void;
  auto remove_child(ViewPtr const&) -> void;

  [[nodiscard]] auto handle_input(InputEvent const&) -> bool;

  struct DrawContext final {
    std::vector<gfx::CommandList>& commandLists;
    Size2Du16 viewport;
  };

  struct UpdateContext final {
    Engine& engine;
    DrawContext const& drawCtx;
    SecondsF32 deltaTime;
  };

  auto update(UpdateContext&) -> void;

protected:
  View() noexcept = default;

  virtual auto on_update(UpdateContext&) -> void;

  [[nodiscard]] virtual auto on_input(InputEvent const&) -> InputEventHandled;

private:
  // top to bottom order
  std::vector<ViewPtr> mChildren;
  InputState mInputState;
};

} // namespace basalt
