#include "transform.h"

#include <basalt/api/math/constants.h>

namespace basalt {

auto Transform::move(const f32 offsetX, const f32 offsetY,
                     const f32 offsetZ) noexcept -> void {
  position += Vector3f32 {offsetX, offsetY, offsetZ};
}

auto Transform::rotate(const Angle offsetX, const Angle offsetY,
                       const Angle offsetZ) noexcept -> void {
  rotation +=
    Vector3f32 {offsetX.radians(), offsetY.radians(), offsetZ.radians()};

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

auto Transform::rotate_y(const Angle offsetY) noexcept -> void {
  rotate(0_rad, offsetY, 0_rad);
}

} // namespace basalt
