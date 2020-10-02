#include "input.h"

#include "shared/utils.h"

using std::vector;

namespace basalt {

auto Input::events() const noexcept -> const vector<InputEventPtr>& {
  return mEvents;
}

auto Input::cursor_position() const noexcept -> CursorPosition {
  return mMousePosition;
}

void Input::mouse_moved(const CursorPosition mousePos) {
  if (mousePos != mMousePosition) {
    mEvents.emplace_back(std::make_unique<MouseMoved>(mousePos));
    mMousePosition = mousePos;
  }
}

void Input::mouse_wheel(const f32 offset) {
  mEvents.emplace_back(std::make_unique<MouseWheel>(offset));
}

auto Input::is_mouse_button_down(const MouseButton button) const -> bool {
  return mMouseButtonsDown[enum_cast(button)];
}

void Input::mouse_button_down(const MouseButton button) {
  if (const auto index = enum_cast(button); !mMouseButtonsDown[index]) {
    mEvents.emplace_back(std::make_unique<MouseButtonDown>(button));
    mMouseButtonsDown[index] = true;
  }
}

void Input::mouse_button_up(const MouseButton button) {
  if (const auto index = enum_cast(button); mMouseButtonsDown[index]) {
    mEvents.emplace_back(std::make_unique<MouseButtonUp>(button));
    mMouseButtonsDown[index] = false;
  }
}

auto Input::is_key_down(const Key key) const -> bool {
  return mKeysDown[enum_cast(key)];
}

void Input::key_down(const Key key) {
  if (const auto index = enum_cast(key); !mKeysDown[index]) {
    mEvents.emplace_back(std::make_unique<KeyDown>(key));
    mKeysDown[index] = true;
  }
}

void Input::key_up(const Key key) {
  if (const auto index = enum_cast(key); mKeysDown[index]) {
    mEvents.emplace_back(std::make_unique<KeyUp>(key));
    mKeysDown[index] = false;
  }
}

void Input::characters_typed(std::string characters) {
  mEvents.emplace_back(std::make_unique<CharactersTyped>(std::move(characters)));
}

} // namespace basalt
