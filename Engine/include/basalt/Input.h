#pragma once
#ifndef BS_INPUT_H
#define BS_INPUT_H

#include "math/Vec2.h"
#include "platform/events/KeyEvents.h" // for Key
#include "platform/events/MouseEvents.h" // for MouseButton

namespace basalt::input {


using platform::Key;
using platform::MouseButton;

void Init();

auto IsKeyPressed(Key key) -> bool;

auto GetMousePos() -> const math::Vec2i32&;

auto IsMouseButtonPressed(MouseButton button) -> bool;

} // namespace basalt::input

#endif // !BS_INPUT_H
