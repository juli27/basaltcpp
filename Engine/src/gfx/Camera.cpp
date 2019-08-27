#include "pch.h"

#include <basalt/gfx/Camera.h>

#include <basalt/math/Constants.h>
#include <basalt/math/Mat4.h>
#include <basalt/math/Vec2.h>
#include <basalt/math/Vec3.h>
#include <basalt/platform/Platform.h>

namespace basalt::gfx {


Camera::Camera(
  const math::Vec3f32& position,
  const math::Vec3f32& lookAt, const math::Vec3f32& up
) : m_position(position)
  , m_lookAt(lookAt)
  , m_up(up)
  , m_dirty(false) {
  const math::Vec2i32 windowSize = platform::get_window_data().mSize;
  const float aspectRatio =
    static_cast<float>(windowSize.GetX()) / windowSize.GetY();
  m_projection = math::Mat4f32::PerspectiveProjection(
    math::PI / 4.0f, aspectRatio, 1.0f, 100.0f
  );

  UpdateView();
}


const math::Mat4f32& Camera::GetViewMatrix() const {
  if (m_dirty) {
    UpdateView();
    m_dirty = false;
  }

  return m_view;
}


const math::Mat4f32& Camera::GetProjectionMatrix() const {
  return m_projection;
}


void Camera::UpdateView() const {
  const math::Vec3f32 zAxis = math::Vec3f32::Normalize(m_lookAt - m_position);
  const math::Vec3f32 xAxis = math::Vec3f32::Normalize(
    math::Vec3f32::Cross(m_up, zAxis)
  );
  const math::Vec3f32 yAxis = math::Vec3f32::Normalize(
    math::Vec3f32::Cross(zAxis, xAxis)
  );

  m_view = math::Mat4::Translation(-m_position) * math::Mat4(
    xAxis.GetX(), yAxis.GetX(), zAxis.GetX(), 0.0f,
    xAxis.GetY(), yAxis.GetY(), zAxis.GetY(), 0.0f,
    xAxis.GetZ(), yAxis.GetZ(), zAxis.GetZ(), 0.0f,
    0.0f   , 0.0f   , 0.0f   , 1.0f
  );
}

} // namespace basalt::gfx
