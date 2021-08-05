#pragma once

#include <basalt/api/base/types.h>

namespace basalt {

struct Mat4;
using Mat4f32 = Mat4;

template <typename T>
struct Rectangle;
using RectangleI16 = Rectangle<i16>;
using RectangleU16 = Rectangle<u16>;

struct Vector3f32;

} // namespace basalt
