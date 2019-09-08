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
) : mPosition(position)
  , mLookAt(lookAt)
  , mUp(up)
  , mDirty(false) {
  const math::Vec2i32 windowSize = platform::get_window_size();
  const float aspectRatio =
    static_cast<float>(windowSize.get_x()) / windowSize.get_y();
  mProjection = math::Mat4f32::perspective_projection(
    math::PI / 4.0f, aspectRatio, 1.0f, 100.0f
  );

  update_view();
}


const math::Mat4f32& Camera::get_view_matrix() const {
  if (mDirty) {
    update_view();
    mDirty = false;
  }

  return mView;
}


const math::Mat4f32& Camera::get_projection_matrix() const {
  return mProjection;
}


void Camera::update_view() const {
  const math::Vec3f32 zAxis = math::Vec3f32::normalize(mLookAt - mPosition);
  const math::Vec3f32 xAxis = math::Vec3f32::normalize(
    math::Vec3f32::cross(mUp, zAxis)
  );
  const math::Vec3f32 yAxis = math::Vec3f32::normalize(
    math::Vec3f32::cross(zAxis, xAxis)
  );

  mView = math::Mat4::translation(-mPosition) * math::Mat4(
    xAxis.get_x(), yAxis.get_x(), zAxis.get_x(), 0.0f,
    xAxis.get_y(), yAxis.get_y(), zAxis.get_y(), 0.0f,
    xAxis.get_z(), yAxis.get_z(), zAxis.get_z(), 0.0f,
    0.0f   , 0.0f   , 0.0f   , 1.0f
  );
}

} // namespace basalt::gfx
