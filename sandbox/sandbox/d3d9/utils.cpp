#include "sandbox/d3d9/utils.h"

#include <runtime/math/Constants.h>
#include <runtime/math/Mat4.h>
#include <runtime/math/Vec3.h>

using basalt::f32;
using basalt::Size2Du16;
using basalt::gfx::Camera;
using basalt::math::Mat4f32;
using basalt::math::PI;
using basalt::math::Vec3f32;

namespace d3d9 {

auto create_default_camera(const Size2Du16 windowSize) -> Camera {
  const Vec3f32 cameraPos {0.0f, 3.0f, -5.0f};
  const Vec3f32 lookAt {0.0f, 0.0f, 0.0f};
  const Vec3f32 up {0.0f, 1.0f, 0.0f};
  const auto aspectRatio {
    static_cast<f32>(windowSize.width()) / static_cast<f32>(windowSize.height())
  };
  const auto projection {
    Mat4f32::perspective_projection(PI / 4.0f, aspectRatio, 1.0f, 100.0f)
  };

  return Camera {cameraPos, lookAt, up, projection};
}

} // namespace d3d9
