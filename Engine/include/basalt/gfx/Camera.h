#pragma once
#ifndef BS_GFX_CAMERA_H
#define BS_GFX_CAMERA_H

#include <basalt/math/Mat4.h>
#include <basalt/math/Vec3.h>

namespace basalt::gfx {

// perspective camera
struct Camera final {
  Camera() = delete;
  Camera(
    const math::Vec3f32& position, const math::Vec3f32& lookAt,
    const math::Vec3f32& up
  );
  Camera(const Camera&) = default;
  Camera(Camera&&) = default;
  ~Camera() = default;

  Camera& operator=(const Camera&) = default;
  Camera& operator=(Camera&&) = default;

  auto view_matrix() const -> const math::Mat4f32&;
  auto projection_matrix() const -> const math::Mat4f32&;

private:
  void update_view() const;

  mutable math::Mat4f32 mView;
  math::Mat4f32 mProjection;
  math::Vec3f32 mPosition;
  math::Vec3f32 mLookAt;
  math::Vec3f32 mUp;
  mutable bool mDirty;
};

} // namespace basalt::gfx

#endif // !BS_GFX_CAMERA_H
