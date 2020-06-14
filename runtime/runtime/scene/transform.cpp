#include "transform.h"

#include <runtime/math/constants.h>

namespace basalt {

void Transform::move(
  const f32 offsetX, const f32 offsetY, const f32 offsetZ) noexcept {
  position += Vec3f32 {offsetX, offsetY, offsetZ};
}

void Transform::rotate(
  const f32 radOffsetX, const f32 radOffsetY, const f32 radOffsetZ) noexcept {
  rotation += Vec3f32 {radOffsetX, radOffsetY, radOffsetZ};

  constexpr auto maxAngle = PI * 2.0f;
  if (rotation.x < 0.0f) {
    rotation.x = maxAngle;
  }
  if (rotation.x > maxAngle) {
    rotation.x = 0.0f;
  }
  if (rotation.y < 0.0f) {
    rotation.y = maxAngle;
  }
  if (rotation.y > maxAngle) {
    rotation.y = 0.0f;
  }
  if (rotation.z < 0.0f) {
    rotation.z = maxAngle;
  }
  if (rotation.z > maxAngle) {
    rotation.z = 0.0f;
  }
}

} // namespace basalt
