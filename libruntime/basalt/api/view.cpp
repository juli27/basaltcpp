#include <basalt/api/view.h>

#include <basalt/api/input_events.h>

#include <basalt/api/base/utils.h>

#include <algorithm>
#include <utility>

using std::vector;

namespace basalt {

auto View::pointer_position() const noexcept -> PointerPosition {
  return mPointerPos;
}

auto View::is_mouse_button_down(const MouseButton button) const -> bool {
  return mMouseButtonsDown[enum_cast(button)];
}

auto View::is_key_down(const Key key) const -> bool {
  return mKeysDown[enum_cast(key)];
}

auto View::add_child_top(ViewPtr view) -> void {
  mChildren.emplace(mChildren.begin(), std::move(view));
}

auto View::add_child_bottom(ViewPtr view) -> void {
  mChildren.emplace_back(std::move(view));
}

auto View::add_child_above(ViewPtr view, const ViewPtr& before) -> void {
  mChildren.emplace(std::find(mChildren.begin(), mChildren.end(), before),
                    std::move(view));
}

auto View::add_child_below(ViewPtr view, const ViewPtr& after) -> void {
  auto it {std::find(mChildren.begin(), mChildren.end(), after)};

  if (it != mChildren.end()) {
    ++it;
  }

  mChildren.emplace(it, std::move(view));
}

auto View::remove_child(const ViewPtr& view) -> void {
  mChildren.erase(std::remove(mChildren.begin(), mChildren.end(), view),
                  mChildren.end());
}

auto View::draw(const DrawContext& context) -> void {
  on_draw(context);

  std::for_each(mChildren.begin(), mChildren.end(),
                [&](const ViewPtr& view) { view->draw(context); });
}

auto View::handle_input(const InputEvent& e) -> bool {
  if (on_input(e) == InputEventHandled::Yes) {
    update(e);

    return true;
  }

  const vector views {mChildren};

  return std::any_of(views.begin(), views.end(), [&](const ViewPtr& view) {
    return view->handle_input(e);
  });
}

auto View::tick(Engine& engine) -> void {
  on_tick(engine);

  const vector views {mChildren};

  for (const ViewPtr& view : views) {
    view->tick(engine);
  }
}

auto View::on_draw(const DrawContext&) -> void {
}

auto View::on_tick(Engine&) -> void {
}

auto View::on_input(const InputEvent&) -> InputEventHandled {
  return InputEventHandled::No;
}

auto View::update(const InputEvent& e) -> void {
  switch (e.type) {
  case InputEventType::MouseMoved:
    mPointerPos = e.as<MouseMoved>().position;
    break;

  case InputEventType::MouseButtonDown:
    mMouseButtonsDown[enum_cast(e.as<MouseButtonDown>().button)] = true;
    break;

  case InputEventType::MouseButtonUp:
    mMouseButtonsDown[enum_cast(e.as<MouseButtonUp>().button)] = false;
    break;

  case InputEventType::KeyDown:
    mKeysDown[enum_cast(e.as<KeyDown>().key)] = true;
    break;

  case InputEventType::KeyUp:
    mKeysDown[enum_cast(e.as<KeyUp>().key)] = false;
    break;

  default:
    break;
  }
}

} // namespace basalt
