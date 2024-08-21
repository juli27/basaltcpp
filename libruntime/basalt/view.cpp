#include <basalt/api/view.h>

#include <algorithm>
#include <utility>

using std::vector;

namespace basalt {

auto View::input_state() const noexcept -> InputState const& {
  return mInputState;
}

auto View::pointer_position() const noexcept -> PointerPosition {
  return mInputState.pointerPos;
}

auto View::is_mouse_button_down(MouseButton const button) const -> bool {
  return mInputState.is_mouse_button_down(button);
}

auto View::is_key_down(Key const key) const -> bool {
  return mInputState.is_key_down(key);
}

auto View::add_child_top(ViewPtr view) -> void {
  mChildren.emplace(mChildren.begin(), std::move(view));
}

auto View::add_child_bottom(ViewPtr view) -> void {
  mChildren.emplace_back(std::move(view));
}

auto View::add_child_above(ViewPtr view, ViewPtr const& before) -> void {
  mChildren.emplace(std::find(mChildren.begin(), mChildren.end(), before),
                    std::move(view));
}

auto View::add_child_below(ViewPtr view, ViewPtr const& after) -> void {
  auto it = std::find(mChildren.begin(), mChildren.end(), after);

  if (it != mChildren.end()) {
    ++it;
  }

  mChildren.emplace(it, std::move(view));
}

auto View::remove_child(ViewPtr const& view) -> void {
  mChildren.erase(std::remove(mChildren.begin(), mChildren.end(), view),
                  mChildren.end());
}

auto View::handle_input(InputEvent const& e) -> bool {
  if (on_input(e) == InputEventHandled::Yes) {
    mInputState.update(e);

    return true;
  }

  auto const views = vector{mChildren};

  return std::any_of(views.begin(), views.end(), [&](ViewPtr const& view) {
    return view->handle_input(e);
  });
}

auto View::update(UpdateContext& ctx) -> void {
  on_update(ctx);

  auto const views = vector{mChildren};

  for (auto const& view : views) {
    view->update(ctx);
  }
}

auto View::on_update(UpdateContext&) -> void {
}

auto View::on_input(InputEvent const&) -> InputEventHandled {
  return InputEventHandled::No;
}

} // namespace basalt
