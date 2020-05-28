#include "runtime/Input.h"

#include "runtime/platform/Platform.h"
#include "runtime/platform/events/Event.h"

using std::vector;

namespace basalt {

using math::Vec2i32;

auto Input::events() const -> const vector<InputEventPtr>& {
  return mEvents;
}

auto Input::mouse_position() const -> Vec2i32 {
  return mMousePosition;
}

void Input::mouse_moved(const i32 x, const i32 y) {
  mEvents.push_back(std::make_unique<MouseMoved>(Vec2i32 {x, y}));
  mMousePosition.set(x, y);
}

auto Input::is_mouse_button_down(const MouseButton button) const -> bool {
  return mMouseButtonsDown[enum_cast(button)];
}

void Input::mouse_button_pressed(const MouseButton button) {
  mEvents.push_back(std::make_unique<MouseButtonPressed>(button));
  mMouseButtonsDown[enum_cast(button)] = true;
}

void Input::mouse_button_released(const MouseButton button) {
  mEvents.push_back(std::make_unique<MouseButtonReleased>(button));
  mMouseButtonsDown[enum_cast(button)] = false;
}

namespace input {
namespace {

std::bitset<KEY_COUNT> sKeyStates;

void on_key_pressed(const platform::KeyPressedEvent& event) {
  const auto index = enum_cast(event.key);
  sKeyStates[index] = true;
}

void on_key_released(const platform::KeyReleasedEvent& event) {
  const auto index = enum_cast(event.key);
  sKeyStates[index] = false;
}

void platform_event_callback(const platform::Event& event) {
  const platform::EventDispatcher dispatcher(event);
  dispatcher.dispatch<platform::KeyPressedEvent>(&on_key_pressed);
  dispatcher.dispatch<platform::KeyReleasedEvent>(&on_key_released);
}

} // namespace


void init() {
  add_event_listener(&platform_event_callback);
}

auto is_key_pressed(const Key key) -> bool {
  return sKeyStates[enum_cast(key)];
}

} // namespace input
} // namespace basalt
