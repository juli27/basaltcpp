#include <basalt/input_manager.h>

#include <basalt/api/input_events.h>
#include <basalt/api/view.h>

#include <basalt/api/base/utils.h>

#include <memory>
#include <utility>

using std::array;

namespace basalt {

void InputManager::set_overlay(ViewPtr overlay) {
  mOverlay = std::move(overlay);
}

void InputManager::dispatch_pending(const ViewPtr& root) {
  for (const InputEventPtr& e : mEvents) {
    if (mOverlay && mOverlay->handle_input(*e)) {
      continue;
    }

    (void)root->handle_input(*e);
  }

  mEvents.clear();
}

void InputManager::mouse_moved(const PointerPosition pointerPos) {
  if (pointerPos != mPointerPos) {
    mPointerPos = pointerPos;
    mEvents.emplace_back(std::make_unique<MouseMoved>(pointerPos));
  }
}

void InputManager::mouse_wheel(const f32 offset) {
  mEvents.emplace_back(std::make_unique<MouseWheel>(offset));
}

void InputManager::mouse_button_down(const MouseButton button) {
  if (const auto index = enum_cast(button); !mMouseButtonsDown[index]) {
    mEvents.emplace_back(std::make_unique<MouseButtonDown>(button));
    mMouseButtonsDown[index] = true;
  }
}

void InputManager::mouse_button_up(const MouseButton button) {
  if (const auto index = enum_cast(button); mMouseButtonsDown[index]) {
    mEvents.emplace_back(std::make_unique<MouseButtonUp>(button));
    mMouseButtonsDown[index] = false;
  }
}

auto InputManager::keyboard_focus_gained() -> void {
  if (!hasKeyboardFocus) {
    hasKeyboardFocus = true;

    mEvents.emplace_back(std::make_unique<KeyboardFocusGained>());
  }
}

auto InputManager::keyboard_focus_lost() -> void {
  if (hasKeyboardFocus) {
    hasKeyboardFocus = false;

    mEvents.emplace_back(std::make_unique<KeyboardFocusLost>());
  }
}


void InputManager::key_down(const Key key) {
  if (const auto index = enum_cast(key); !mKeysDown[index]) {
    mKeysDown[index] = true;
    mEvents.emplace_back(std::make_unique<KeyDown>(key));
  }
}

void InputManager::key_up(const Key key) {
  if (const auto index = enum_cast(key); mKeysDown[index]) {
    mKeysDown[index] = false;
    mEvents.emplace_back(std::make_unique<KeyUp>(key));
  }
}

void InputManager::character_utf8(std::array<char, 4> character) {
  mEvents.emplace_back(std::make_unique<CharacterTyped>(character));
}

} // namespace basalt
