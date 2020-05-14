#include "runtime/gfx/backend/Types.h"

namespace basalt::gfx::backend {

void LightSetup::add_directional_light(
  const math::Vec3f32& direction, const Color& diffuseColor
) {
  mDirectionalLights.push_back(DirectionalLight {direction, diffuseColor});
}

auto LightSetup::directional_lights() const -> const std::vector<
  DirectionalLight>& {
  return mDirectionalLights;
}

} // namespace basalt::gfx::backend
