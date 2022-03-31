#include <basalt/sandbox/d3d9/utils.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/vector3.h>

using namespace basalt::literals;

using basalt::Vector3f32;
using basalt::gfx::Camera;

namespace d3d9 {

auto create_default_camera() noexcept -> Camera {
  constexpr Vector3f32 cameraPos {0.0f, 3.0f, -5.0f};
  constexpr Vector3f32 lookAt {0.0f};

  return Camera {cameraPos, lookAt, Vector3f32::up(), 45.0_deg, 1.0f, 100.0f};
}

} // namespace d3d9
