#include "runtime/Input.h"

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

void Input::mouse_wheel(const f32 offset) {
  mEvents.push_back(std::make_unique<MouseWheel>(offset));
}

auto Input::is_mouse_button_down(const MouseButton button) const -> bool {
  return mMouseButtonsDown[enum_cast(button)];
}

void Input::mouse_button_down(const MouseButton button) {
  mEvents.push_back(std::make_unique<MouseButtonDown>(button));
  mMouseButtonsDown[enum_cast(button)] = true;
}

void Input::mouse_button_up(const MouseButton button) {
  mEvents.push_back(std::make_unique<MouseButtonUp>(button));
  mMouseButtonsDown[enum_cast(button)] = false;
}

auto Input::is_key_down(const Key key) const -> bool {
  return mKeysDown[enum_cast(key)];
}

void Input::key_down(const Key key) {
  mEvents.push_back(std::make_unique<KeyDown>(key));
  mKeysDown[enum_cast(key)] = true;
}

void Input::key_up(const Key key) {
  mEvents.push_back(std::make_unique<KeyUp>(key));
  mKeysDown[enum_cast(key)] = false;
}

void Input::characters_typed(std::string characters) {
  mEvents.push_back(std::make_unique<CharactersTyped>(std::move(characters)));
}

} // namespace basalt
