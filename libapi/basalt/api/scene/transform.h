#pragma once

#include <basalt/api/math/angle.h>
#include <basalt/api/math/vector3.h>

#include <basalt/api/base/types.h>

namespace basalt {

struct Transform {
  // TODO: wrap vectors in Position/Rotation/Scale class?
  void move(f32 offsetX, f32 offsetY, f32 offsetZ) noexcept;
  void rotate(Angle offsetX, Angle offsetY, Angle offsetZ) noexcept;

  Vector3f32 position {0.0f};
  Vector3f32 rotation {0.0f};
  Vector3f32 scale {1.0f};
};

} // namespace basalt
