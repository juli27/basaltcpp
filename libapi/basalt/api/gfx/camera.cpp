#include <basalt/api/gfx/camera.h>

#include <basalt/api/math/mat4.h>

#include <basalt/api/shared/size2d.h>

namespace basalt::gfx {

Camera::Camera(const Vector3f32& position, const Vector3f32& lookAt,
               const Vector3f32& up, const Angle fov, const f32 nearPlane,
               const f32 farPlane) noexcept
  : mPosition {position}
  , mLookAt {lookAt}
  , mUp {up}
  , mFov {fov}
  , mNearPlane {nearPlane}
  , mFarPlane {farPlane} {
}

auto Camera::view_matrix() const noexcept -> Mat4f32 {
  const auto zAxis = Vector3f32::normalize(mLookAt - mPosition);
  const auto xAxis = Vector3f32::normalize(Vector3f32::cross(mUp, zAxis));
  const auto yAxis = Vector3f32::normalize(Vector3f32::cross(zAxis, xAxis));

  return Mat4f32::translation(-mPosition) *
         // clang-format off
         Mat4f32 {xAxis.x(), yAxis.x(), zAxis.x(), 0.0f,
                  xAxis.y(), yAxis.y(), zAxis.y(), 0.0f,
                  xAxis.z(), yAxis.z(), zAxis.z(), 0.0f,
                       0.0f,      0.0f,      0.0f, 1.0f};
  // clang-format on
}

auto Camera::projection_matrix(const Size2Du16 viewport) const noexcept
  -> Mat4f32 {
  const auto aspectRatio {static_cast<f32>(viewport.width()) /
                          static_cast<f32>(viewport.height())};

  return Mat4f32::perspective_projection(mFov, aspectRatio, mNearPlane,
                                         mFarPlane);
}

} // namespace basalt::gfx
