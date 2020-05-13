#pragma once
#ifndef BASALT_GFX_CAMERA_H
#define BASALT_GFX_CAMERA_H

#include "runtime/math/Mat4.h"
#include "runtime/math/Vec3.h"

namespace basalt::gfx {

// perspective camera
struct Camera final {
  Camera() = delete;

  Camera(
    const math::Vec3f32& position, const math::Vec3f32& lookAt
  , const math::Vec3f32& up
  );

  Camera(
    const math::Vec3f32& position, const math::Vec3f32& lookAt
  , const math::Vec3f32& up, const math::Mat4f32& projection
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

  math::Mat4f32 mProjection {};
  mutable math::Mat4f32 mView {};
  math::Vec3f32 mPosition {};
  math::Vec3f32 mLookAt {};
  math::Vec3f32 mUp {};
  mutable bool mDirty {false};
};

} // namespace basalt::gfx

#endif // !BASALT_GFX_CAMERA_H
