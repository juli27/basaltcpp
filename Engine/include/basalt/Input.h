#pragma once
#ifndef BS_INPUT_H
#define BS_INPUT_H

#include "common/Types.h"
#include "math/Vec2.h"

namespace basalt {
namespace input {

void Init();

bool IsKeyPressed(Key key);

const math::Vec2i32& GetMousePos();

bool IsMouseButtonPressed(MouseButton button);

} // namespace input
} // namespace basalt

#endif // !BS_INPUT_H
