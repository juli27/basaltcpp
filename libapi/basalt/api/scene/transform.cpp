#include "transform.h"

#include <basalt/api/math/angle.h>

namespace basalt {

auto Transform::move(f32 const offsetX, f32 const offsetY,
                     f32 const offsetZ) noexcept -> void {
  position += Vector3f32{offsetX, offsetY, offsetZ};
}

auto Transform::rotate(Angle const offsetX, Angle const offsetY,
                       Angle const offsetZ) noexcept -> void {
  rotation +=
    Vector3f32{offsetX.radians(), offsetY.radians(), offsetZ.radians()};

  // this little dance is to (ab-) use the angle normalization
  rotation.x() = Angle::radians(rotation.x()).radians();
  rotation.y() = Angle::radians(rotation.y()).radians();
  rotation.z() = Angle::radians(rotation.z()).radians();
}

auto Transform::rotate_y(Angle const offsetY) noexcept -> void {
  rotate(0_rad, offsetY, 0_rad);
}

auto Transform::to_matrix() const -> Matrix4x4f32 {
  return Matrix4x4f32::scaling(scale) * Matrix4x4f32::rotation(rotation) *
         Matrix4x4f32::translation(position);
}

} // namespace basalt
