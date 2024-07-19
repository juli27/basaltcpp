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

auto InputState::update(InputEvent const& inputEvent) -> void {
  handle(
    inputEvent,
    InputEventHandler{
      [this](MouseMoved const& mouseMoved) {
        pointerPos = mouseMoved.position;
      },
      [this](MouseButtonDown const& mouseButtonDown) {
        mouseButtonsDown[enum_cast(mouseButtonDown.button)] = true;
      },
      [this](MouseButtonUp const& mouseButtonUp) {
        mouseButtonsDown[enum_cast(mouseButtonUp.button)] = false;
      },
      [this](KeyDown const& keyDown) {
        keysDown[enum_cast(keyDown.key)] = true;
      },
      [this](KeyUp const& keyUp) { keysDown[enum_cast(keyUp.key)] = false; },
      ignoreOtherEvents,
    });
}

} // namespace basalt
