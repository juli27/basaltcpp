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

void View::add_child_top(ViewPtr view) {
  mChildren.emplace(mChildren.begin(), std::move(view));
}

void View::add_child_bottom(ViewPtr view) {
  mChildren.emplace_back(std::move(view));
}

void View::add_child_above(ViewPtr view, const ViewPtr& before) {
  mChildren.emplace(std::find(mChildren.begin(), mChildren.end(), before),
                    std::move(view));
}

void View::add_child_below(ViewPtr view, const ViewPtr& after) {
  auto it {std::find(mChildren.begin(), mChildren.end(), after)};

  if (it != mChildren.end()) {
    ++it;
  }

  mChildren.emplace(it, std::move(view));
}

void View::remove_child(const ViewPtr& view) {
  mChildren.erase(std::remove(mChildren.begin(), mChildren.end(), view),
                  mChildren.end());
}

void View::draw(const DrawContext& context) {
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

void View::tick(Engine& engine) {
  on_tick(engine);

  const vector views {mChildren};

  for (const ViewPtr& view : views) {
    view->tick(engine);
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
