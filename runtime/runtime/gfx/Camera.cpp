#include "runtime/gfx/Camera.h"

#include "runtime/platform/Platform.h"

#include "runtime/math/Constants.h"

namespace basalt::gfx {

Camera::Camera(const math::Vec3f32& position, const math::Vec3f32& lookAt
             , const math::Vec3f32& up)
  : mPosition(position)
  , mLookAt(lookAt)
  , mUp(up) {
  const auto windowSize{ platform::get_window_size() };
  const auto aspectRatio{
    static_cast<float>(windowSize.width()) /
    static_cast<float>(windowSize.height()) };
  mProjection = math::Mat4f32::perspective_projection(math::PI / 4.0f
                                                    , aspectRatio, 1.0f
                                                    , 100.0f);

  update_view();
}

auto Camera::view_matrix() const -> const math::Mat4f32& {
  if (mDirty) {
    update_view();
    mDirty = false;
  }

  return mView;
}

auto Camera::projection_matrix() const -> const math::Mat4f32& {
  return mProjection;
}

void Camera::update_view() const {
  const auto zAxis = math::Vec3f32::normalize(mLookAt - mPosition);
  const auto xAxis = math::Vec3f32::normalize(math::Vec3f32::cross(mUp, zAxis));
  const auto yAxis =
    math::Vec3f32::normalize(math::Vec3f32::cross(zAxis, xAxis));

  mView = math::Mat4::translation(-mPosition) * math::Mat4(
            xAxis.x(), yAxis.x(), zAxis.x(), 0.0f,
            xAxis.y(), yAxis.y(), zAxis.y(), 0.0f,
            xAxis.z(), yAxis.z(), zAxis.z(), 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
          );
}

} // namespace basalt::gfx
