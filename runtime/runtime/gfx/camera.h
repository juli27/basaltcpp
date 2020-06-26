#pragma once

#include <runtime/math/mat4.h>
#include <runtime/math/vec3.h>

#include <runtime/shared/types.h>

namespace basalt {

template <typename T>
struct Size2D;
using Size2Du16 = Size2D<u16>;

namespace gfx {

// perspective camera
struct Camera final {
  Camera() = default;

  Camera(
    const Vec3f32& position, const Vec3f32& lookAt, const Vec3f32& up, f32 fov
  , f32 nearPlane, f32 farPlane);

  Camera(const Camera&) = default;
  Camera(Camera&&) = default;

  ~Camera() = default;

  Camera& operator=(const Camera&) = default;
  Camera& operator=(Camera&&) = default;

  [[nodiscard]]
  auto view_matrix() const -> Mat4f32;

  [[nodiscard]]
  auto projection_matrix(Size2Du16 viewport) const -> Mat4f32;

private:
  Vec3f32 mPosition {};
  Vec3f32 mLookAt {};
  Vec3f32 mUp {};
  f32 mFov {};
  f32 mNearPlane {};
  f32 mFarPlane {};
};

} // namespace gfx
} // namespace basalt