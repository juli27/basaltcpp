#pragma once

#include "api/math/types.h"
#include "api/math/vector3.h"

#include "api/shared/types.h"

#include "api/base/types.h"

namespace basalt::gfx {

// perspective camera
struct Camera final {
  Camera(const Vector3f32& position, const Vector3f32& lookAt,
         const Vector3f32& up, f32 fov, f32 nearPlane, f32 farPlane) noexcept;

  [[nodiscard]] auto view_matrix() const noexcept -> Mat4f32;

  [[nodiscard]] auto projection_matrix(Size2Du16 viewport) const noexcept
    -> Mat4f32;

private:
  Vector3f32 mPosition;
  Vector3f32 mLookAt;
  Vector3f32 mUp;
  f32 mFov {};
  f32 mNearPlane {};
  f32 mFarPlane {};
};

} // namespace basalt::gfx
