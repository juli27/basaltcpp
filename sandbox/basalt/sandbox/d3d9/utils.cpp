#include <basalt/sandbox/d3d9/utils.h>

#include <basalt/api/math/constants.h>
#include <basalt/api/math/vector3.h>

using basalt::PI;
using basalt::Vector3f32;
using basalt::gfx::Camera;

namespace d3d9 {

auto create_default_camera() -> Camera {
  constexpr Vector3f32 cameraPos {0.0f, 3.0f, -5.0f};
  constexpr Vector3f32 lookAt {};
  constexpr Vector3f32 up {0.0f, 1.0f, 0.0f};

  return Camera {cameraPos, lookAt, up, PI / 4.0f, 1.0f, 100.0f};
}

} // namespace d3d9
