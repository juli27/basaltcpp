#pragma once
#ifndef BS_GFX_CAMERA_H
#define BS_GFX_CAMERA_H

#include <basalt/math/Mat4.h>
#include <basalt/math/Vec3.h>

namespace basalt {
namespace gfx {


// perspective camera
class Camera final {
public:

  Camera(
    const math::Vec3f32& position, const math::Vec3f32& lookAt,
    const math::Vec3f32& up
  );

  Camera(const Camera&) = default;
  Camera(Camera&&) = default;
  ~Camera() = default;

  Camera() = delete;

public:

  const math::Mat4f32& GetViewMatrix() const;


  const math::Mat4f32& GetProjectionMatrix() const;

private:

  void UpdateView() const;

public:
  Camera& operator=(const Camera&) = default;
  Camera& operator=(Camera&&) = default;

private:
  mutable math::Mat4f32 m_view;
  math::Mat4f32 m_projection;
  math::Vec3f32 m_position;
  math::Vec3f32 m_lookAt;
  math::Vec3f32 m_up;
  mutable bool m_dirty;
};

} // namespace gfx
} // namespace basalt

#endif // !BS_GFX_CAMERA_H
