#include <basalt/api/view.h>

#include <basalt/api/input_events.h>

#include <basalt/api/base/utils.h>

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

auto View::handle_input(const InputEvent& e) -> bool {
  if (on_input(e) == InputEventHandled::Yes) {
    update(e);

    return true;
  }

  return false;
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
