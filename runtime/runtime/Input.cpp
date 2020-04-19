#include "runtime/Input.h"

#include "runtime/platform/Platform.h"
#include "runtime/platform/events/Event.h"

#include <bitset>

namespace basalt::input {
namespace {

std::bitset<KEY_COUNT> sKeyStates;
std::bitset<MOUSE_BUTTON_COUNT> sButtonStates;
math::Vec2i32 sMousePos;

void on_key_pressed(const platform::KeyPressedEvent& event) {
  const auto index = enum_cast(event.mKey);
  sKeyStates[index] = true;
}

void on_key_released(const platform::KeyReleasedEvent& event) {
  const auto index = enum_cast(event.mKey);
  sKeyStates[index] = false;
}

void on_mouse_moved(const platform::MouseMovedEvent& event) {
  sMousePos = event.mPos;
}

void on_button_pressed(const platform::MouseButtonPressedEvent& event) {
  const auto index = enum_cast(event.mButton);
  sButtonStates[index] = true;
}

void on_button_released(const platform::MouseButtonReleasedEvent& event) {
  const auto index = enum_cast(event.mButton);
  sButtonStates[index] = false;
}

void platform_event_callback(const platform::Event& event) {
  const platform::EventDispatcher dispatcher(event);
  dispatcher.dispatch<platform::MouseMovedEvent>(&on_mouse_moved);
  dispatcher.dispatch<platform::KeyPressedEvent>(&on_key_pressed);
  dispatcher.dispatch<platform::KeyReleasedEvent>(&on_key_released);
  dispatcher.dispatch<platform::MouseButtonPressedEvent>(&on_button_pressed);
  dispatcher.dispatch<platform::MouseButtonReleasedEvent>(&on_button_released);
}

} // namespace


void init() {
  add_event_listener(&platform_event_callback);
}

auto is_key_pressed(const Key key) -> bool {
  return sKeyStates[enum_cast(key)];
}

auto get_mouse_pos() -> const math::Vec2i32& {
  return sMousePos;
}

auto is_mouse_button_pressed(const MouseButton button) -> bool {
  return sButtonStates[enum_cast(button)];
}

} // namespace basalt::input
