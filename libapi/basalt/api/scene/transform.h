#pragma once

#include <basalt/api/math/angle.h>
#include <basalt/api/math/matrix4x4.h>
#include <basalt/api/math/vector3.h>

#include <basalt/api/base/types.h>

namespace basalt {

struct Transform {
  // TODO: wrap vectors in Position/Rotation/Scale class?
  auto move(f32 offsetX, f32 offsetY, f32 offsetZ) noexcept -> void;
  auto rotate(Angle offsetX, Angle offsetY, Angle offsetZ) noexcept -> void;
  auto rotate_y(Angle offsetY) noexcept -> void;

  Vector3f32 position {0.0f};
  Vector3f32 rotation {0.0f};
  Vector3f32 scale {1.0f};
};

struct LocalToWorld {
  Matrix4x4f32 value;
};

} // namespace basalt
