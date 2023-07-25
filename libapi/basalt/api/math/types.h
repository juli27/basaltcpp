#pragma once

#include <basalt/api/base/types.h>

namespace basalt {

class Angle;

struct Matrix4x4f32;

template <typename T>
struct Rectangle;
using RectangleI16 = Rectangle<i16>;
using RectangleU16 = Rectangle<u16>;

class Vector2f32;
class Vector3f32;
class Vector4f32;

} // namespace basalt
