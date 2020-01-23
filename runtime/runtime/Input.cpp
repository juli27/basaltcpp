#include "runtime/Input.h"

#include "runtime/platform/Platform.h"
#include "runtime/platform/events/Event.h"
#include "runtime/shared/Types.h"

#include <bitset>

namespace basalt::input {
namespace {

constexpr i32 NUM_KEYS = static_cast<i32>(Key::NumberOfKeys);
constexpr i32 NUM_MOUSE_BUTTONS = static_cast<i32>(
  MouseButton::NumberOfButtons
);

std::bitset<NUM_KEYS> sKeyStates;
std::bitset<NUM_MOUSE_BUTTONS> sButtonStates;
math::Vec2i32 sMousePos;

void on_key_pressed(const platform::KeyPressedEvent& event) {
  const auto index = static_cast<i32>(event.mKey);
  sKeyStates[index] = true;
}

void on_key_released(const platform::KeyReleasedEvent& event) {
  const auto index = static_cast<i32>(event.mKey);
  sKeyStates[index] = false;
}

void on_mouse_moved(const platform::MouseMovedEvent& event) {
  sMousePos = event.mPos;
}

void on_button_pressed(const platform::MouseButtonPressedEvent& event) {
  const auto index = static_cast<i32>(event.mButton);
  sButtonStates[index] = true;
}

void on_button_released(const platform::MouseButtonReleasedEvent& event) {
  const auto index = static_cast<i32>(event.mButton);
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

auto is_key_pressed(Key key) -> bool {
  return sKeyStates[static_cast<i32>(key)];
}

auto get_mouse_pos() -> const math::Vec2i32& {
  return sMousePos;
}

auto is_mouse_button_pressed(MouseButton button) -> bool {
  return sButtonStates[static_cast<i32>(button)];
}

} // namespace basalt::input
