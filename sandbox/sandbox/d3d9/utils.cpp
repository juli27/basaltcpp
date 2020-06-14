#include "utils.h"

#include <runtime/math/constants.h>
#include <runtime/math/mat4.h>
#include <runtime/math/vec3.h>

using basalt::f32;
using basalt::Mat4f32;
using basalt::PI;
using basalt::Size2Du16;
using basalt::Vec3f32;
using basalt::gfx::Camera;

namespace d3d9 {

auto create_default_camera() -> Camera {
  const Vec3f32 cameraPos {0.0f, 3.0f, -5.0f};
  const Vec3f32 lookAt {0.0f, 0.0f, 0.0f};
  const Vec3f32 up {0.0f, 1.0f, 0.0f};

  return Camera {cameraPos, lookAt, up, PI / 4.0f, 1.0f, 100.0f};
}

} // namespace d3d9
