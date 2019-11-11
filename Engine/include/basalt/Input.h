#pragma once
#ifndef BASALT_INPUT_H
#define BASALT_INPUT_H

#include "platform/events/KeyEvents.h" // for Key
#include "platform/events/MouseEvents.h" // for MouseButton

#include "math/Vec2.h"

namespace basalt::input {

using platform::Key;
using platform::MouseButton;

void init();

auto is_key_pressed(Key key) -> bool;

auto get_mouse_pos() -> const math::Vec2i32&;
auto is_mouse_button_pressed(MouseButton button) -> bool;

} // namespace basalt::input

#endif // !BASALT_INPUT_H
