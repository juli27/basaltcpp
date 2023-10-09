#include <basalt/input_manager.h>

#include <basalt/api/input_events.h>
#include <basalt/api/view.h>

#include <basalt/api/base/utils.h>

#include <memory>
#include <utility>

using std::array;

namespace basalt {

auto InputManager::set_overlay(ViewPtr overlay) -> void {
  mOverlay = std::move(overlay);
}

auto InputManager::dispatch_pending(ViewPtr const& root) -> void {
  for (auto const& e : mEvents) {
    if (mOverlay && mOverlay->handle_input(*e)) {
      continue;
    }

    (void)root->handle_input(*e);
  }

  mEvents.clear();
}

auto InputManager::mouse_moved(PointerPosition const pointerPos) -> void {
  if (pointerPos != mPointerPos) {
    mPointerPos = pointerPos;
    mEvents.emplace_back(std::make_unique<MouseMoved>(pointerPos));
  }
}

auto InputManager::mouse_wheel(f32 const offset) -> void {
  mEvents.emplace_back(std::make_unique<MouseWheel>(offset));
}

auto InputManager::mouse_button_down(MouseButton const button) -> void {
  if (auto const index = enum_cast(button); !mMouseButtonsDown[index]) {
    mEvents.emplace_back(std::make_unique<MouseButtonDown>(button));
    mMouseButtonsDown[index] = true;
  }
}

auto InputManager::mouse_button_up(MouseButton const button) -> void {
  if (auto const index = enum_cast(button); mMouseButtonsDown[index]) {
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

auto InputManager::key_down(Key const key) -> void {
  if (auto const index = enum_cast(key); !mKeysDown[index]) {
    mKeysDown[index] = true;
    mEvents.emplace_back(std::make_unique<KeyDown>(key));
  }
}

auto InputManager::key_up(Key const key) -> void {
  if (auto const index = enum_cast(key); mKeysDown[index]) {
    mKeysDown[index] = false;
    mEvents.emplace_back(std::make_unique<KeyUp>(key));
  }
}

auto InputManager::character_utf8(std::array<char, 4> character) -> void {
  mEvents.emplace_back(std::make_unique<CharacterTyped>(character));
}

} // namespace basalt
