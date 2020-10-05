#pragma once

#include "api/math/vec3.h"
#include "api/shared/types.h"

namespace basalt {

struct Transform {
  // TODO: wrap vectors in Position/Rotation/Scale class?
  void move(f32 offsetX, f32 offsetY, f32 offsetZ) noexcept;
  void rotate(f32 radOffsetX, f32 radOffsetY, f32 radOffsetZ) noexcept;

  Vec3f32 position {};
  Vec3f32 rotation {};
  Vec3f32 scale {1.0f, 1.0f, 1.0f};
};

} // namespace basalt
