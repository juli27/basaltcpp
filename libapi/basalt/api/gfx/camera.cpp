#include <basalt/api/gfx/camera.h>

#include <basalt/api/math/matrix4x4.h>

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

auto Camera::world_to_view() const noexcept -> Matrix4x4f32 {
  return Matrix4x4f32::look_at_lh(mPosition, mLookAt, mUp);
}

auto Camera::view_to_clip(const Size2Du16 viewport) const noexcept
  -> Matrix4x4f32 {
  const f32 aspectRatio {viewport.aspect_ratio()};

  return Matrix4x4f32::perspective_projection(mFov, aspectRatio, mNearPlane,
                                              mFarPlane);
}

} // namespace basalt::gfx
