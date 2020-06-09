#pragma once

#include "runtime/math/Mat4.h"
#include "runtime/math/Vec3.h"

#include <runtime/shared/Size2D.h>
#include <runtime/shared/Types.h>

namespace basalt::gfx {

// perspective camera
struct Camera final {
  Camera() = default;

  Camera(
    const math::Vec3f32& position, const math::Vec3f32& lookAt
  , const math::Vec3f32& up, f32 fov, f32 nearPlane, f32 farPlane);

  Camera(const Camera&) = default;
  Camera(Camera&&) = default;

  ~Camera() = default;

  Camera& operator=(const Camera&) = default;
  Camera& operator=(Camera&&) = default;

  [[nodiscard]]
  auto view_matrix() const -> math::Mat4f32;

  [[nodiscard]]
  auto projection_matrix(Size2Du16 viewport) const -> math::Mat4f32;

private:
  math::Vec3f32 mPosition {};
  math::Vec3f32 mLookAt {};
  math::Vec3f32 mUp {};
  f32 mFov {};
  f32 mNearPlane {};
  f32 mFarPlane {};
};

} // namespace basalt::gfx
