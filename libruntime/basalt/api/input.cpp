#include <basalt/api/input.h>

#include <basalt/api/input_events.h>

#include <basalt/api/base/utils.h>

namespace basalt {

auto InputState::is_mouse_button_down(MouseButton const button) const -> bool {
  return mouseButtonsDown[enum_cast(button)];
}

auto InputState::is_key_down(Key const key) const -> bool {
  return keysDown[enum_cast(key)];
}

auto InputState::update(InputEvent const& e) -> void {
  switch (e.type) {
  case InputEventType::MouseMoved:
    pointerPos = e.as<MouseMoved>().position;
    break;

  case InputEventType::MouseButtonDown:
    mouseButtonsDown[enum_cast(e.as<MouseButtonDown>().button)] = true;
    break;

  case InputEventType::MouseButtonUp:
    mouseButtonsDown[enum_cast(e.as<MouseButtonUp>().button)] = false;
    break;

  case InputEventType::KeyDown:
    keysDown[enum_cast(e.as<KeyDown>().key)] = true;
    break;

  case InputEventType::KeyUp:
    keysDown[enum_cast(e.as<KeyUp>().key)] = false;
    break;

  default:
    break;
  }
}

} // namespace basalt
