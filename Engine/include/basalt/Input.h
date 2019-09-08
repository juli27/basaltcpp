#pragma once
#ifndef BS_INPUT_H
#define BS_INPUT_H

#include "math/Vec2.h"
#include "platform/events/KeyEvents.h" // for Key
#include "platform/events/MouseEvents.h" // for MouseButton

namespace basalt::input {


using platform::Key;
using platform::MouseButton;

void init();

auto is_key_pressed(Key key) -> bool;

auto get_mouse_pos() -> const math::Vec2i32&;

auto is_mouse_button_pressed(MouseButton button) -> bool;

} // namespace basalt::input

#endif // !BS_INPUT_H
