#pragma once

#include <basalt/api/input_events.h>
#include <basalt/api/types.h>

#include <basalt/api/base/types.h>

#include <array>
#include <bitset>
#include <vector>

namespace basalt {

// TODO: mouse leave
// TODO: mouse capturing
struct InputManager final {
  void set_overlay(InputLayerPtr);

  void dispatch_pending(const std::vector<InputLayerPtr>& dispatchChain);

  void mouse_moved(PointerPosition);

  void mouse_wheel(f32 offset);

  void mouse_button_down(MouseButton);
  void mouse_button_up(MouseButton);

  void key_down(Key);
  void key_up(Key);

  void character_utf8(std::array<char, 4>);

private:
  InputLayerPtr mOverlay;
  std::vector<InputEventPtr> mEvents;
  PointerPosition mPointerPos;
  std::bitset<MOUSE_BUTTON_COUNT> mMouseButtonsDown;
  std::bitset<KEY_COUNT> mKeysDown;
};

} // namespace basalt
