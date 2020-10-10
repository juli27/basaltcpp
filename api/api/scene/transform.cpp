#include "transform.h"

#include "api/math/constants.h"

namespace basalt {

void Transform::move(const f32 offsetX, const f32 offsetY,
                     const f32 offsetZ) noexcept {
  position += Vector3f32 {offsetX, offsetY, offsetZ};
}

void Transform::rotate(const f32 radOffsetX, const f32 radOffsetY,
                       const f32 radOffsetZ) noexcept {
  rotation += Vector3f32 {radOffsetX, radOffsetY, radOffsetZ};

  constexpr auto twoPi = PI * 2.0f;
  if (rotation.x() < -PI) {
    rotation.x() += twoPi;
  }
  if (rotation.x() > PI) {
    rotation.x() -= twoPi;
  }
  if (rotation.y() < -PI) {
    rotation.y() += twoPi;
  }
  if (rotation.y() > PI) {
    rotation.y() -= twoPi;
  }
  if (rotation.z() < -PI) {
    rotation.z() += twoPi;
  }
  if (rotation.z() > PI) {
    rotation.z() -= twoPi;
  }
}

} // namespace basalt
