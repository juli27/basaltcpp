#include "input_manager.h"

#include <api/input_events.h>
#include <api/input_layer.h>

#include <api/base/utils.h>

#include <memory>
#include <utility>

using std::array;

namespace basalt {

void InputManager::set_overlay(InputLayerPtr overlay) {
  mOverlay = std::move(overlay);
}

void InputManager::dispatch_pending(
  const std::vector<InputLayerPtr>& dispatchChain) {
  for (const InputEventPtr& e : mEvents) {
    if (mOverlay && mOverlay->handle_input(*e)) {
      continue;
    }

    for (const InputLayerPtr& inputTarget : dispatchChain) {
      if (inputTarget->handle_input(*e)) {
        break;
      }
    }
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
