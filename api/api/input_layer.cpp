#include "input_layer.h"

#include "input_events.h"

#include "base/utils.h"

namespace basalt {

auto InputLayer::pointer_position() const noexcept -> PointerPosition {
  return mPointerPos;
}

auto InputLayer::is_mouse_button_down(const MouseButton button) const -> bool {
  return mMouseButtonsDown[enum_cast(button)];
}

auto InputLayer::is_key_down(const Key key) const -> bool {
  return mKeysDown[enum_cast(key)];
}

auto InputLayer::handle_input(const InputEvent& e) -> bool {
  if (do_handle_input(e) == InputEventHandled::Yes) {
    update(e);

    return true;
  }

  return false;
}

void InputLayer::update(const InputEvent& e) {
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
