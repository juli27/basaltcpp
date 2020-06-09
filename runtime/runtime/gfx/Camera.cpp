#include "runtime/gfx/Camera.h"

namespace basalt::gfx {

using math::Mat4f32;
using math::Vec3f32;

Camera::Camera(
  const Vec3f32& position, const Vec3f32& lookAt, const Vec3f32& up
, const f32 fov, const f32 nearPlane, const f32 farPlane
)
  : mPosition(position)
  , mLookAt(lookAt)
  , mUp(up)
  , mFov {fov}
  , mNearPlane {nearPlane}
  , mFarPlane {farPlane} {
}

auto Camera::view_matrix() const -> Mat4f32 {
  const auto zAxis = Vec3f32::normalize(mLookAt - mPosition);
  const auto xAxis = Vec3f32::normalize(Vec3f32::cross(mUp, zAxis));
  const auto yAxis = Vec3f32::normalize(Vec3f32::cross(zAxis, xAxis));

  return Mat4f32::translation(-mPosition) * Mat4f32 {
    xAxis.x, yAxis.x, zAxis.x, 0.0f
  , xAxis.y, yAxis.y, zAxis.y, 0.0f
  , xAxis.z, yAxis.z, zAxis.z, 0.0f
  , 0.0f, 0.0f, 0.0f, 1.0f
  };
}

auto Camera::projection_matrix(const Size2Du16 viewport) const -> Mat4f32 {
  const auto aspectRatio {
    static_cast<f32>(viewport.width()) / static_cast<f32>(viewport.height())
  };

  return Mat4f32::perspective_projection(
    mFov, aspectRatio, mNearPlane, mFarPlane);
}

} // namespace basalt::gfx
