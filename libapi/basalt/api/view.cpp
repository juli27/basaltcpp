#include <basalt/api/view.h>

#include <basalt/api/input_events.h>

#include <basalt/api/base/utils.h>

#include <algorithm>
#include <iterator>
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

void View::add_child_top(ViewPtr view) {
  mViews.emplace(mViews.begin(), std::move(view));
}

void View::add_child_bottom(ViewPtr view) {
  mViews.emplace_back(std::move(view));
}

void View::add_child_above(ViewPtr view, const ViewPtr& before) {
  mViews.emplace(std::find(mViews.begin(), mViews.end(), before),
                 std::move(view));
}

void View::add_child_below(ViewPtr view, const ViewPtr& after) {
  auto it {std::find(mViews.begin(), mViews.end(), after)};

  if (it != mViews.end()) {
    ++it;
  }

  mViews.emplace(it, std::move(view));
}

void View::remove_child(const ViewPtr& view) {
  mViews.erase(std::remove(mViews.begin(), mViews.end(), view), mViews.end());
}

auto View::handle_input(const InputEvent& e) -> bool {
  if (on_input(e) == InputEventHandled::Yes) {
    update(e);

    return true;
  }

  const vector views {mViews};

  return std::any_of(views.begin(), views.end(), [&](const ViewPtr& view) {
    return view->handle_input(e);
  });
}

void View::tick(Engine& engine) {
  on_tick(engine);

  const vector views {mViews};

  for (const ViewPtr& view : views) {
    view->on_tick(engine);
  }
}

void View::update(const InputEvent& e) {
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
