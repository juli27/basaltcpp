#pragma once

#include <basalt/api/shared/types.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/types.h>
#include <basalt/api/math/vector3.h>

#include <basalt/api/base/types.h>

namespace basalt::gfx {

// perspective camera
struct Camera final {
  Camera(const Vector3f32& position, const Vector3f32& lookAt,
         const Vector3f32& up, Angle fov, f32 nearPlane, f32 farPlane) noexcept;

  [[nodiscard]] auto world_to_view() const noexcept -> Matrix4x4f32;

  [[nodiscard]] auto view_to_viewport(Size2Du16 viewport) const noexcept
    -> Matrix4x4f32;

private:
  Vector3f32 mPosition;
  Vector3f32 mLookAt;
  Vector3f32 mUp;
  Angle mFov;
  f32 mNearPlane {};
  f32 mFarPlane {};
};

} // namespace basalt::gfx
