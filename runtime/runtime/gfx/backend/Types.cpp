#include "runtime/gfx/backend/Types.h"

namespace basalt::gfx::backend {

void LightSetup::add_directional_light(
  const math::Vec3f32& direction, const Color& diffuseColor
) {
  mDirectionalLights.push_back(DirectionalLight {direction, diffuseColor});
}

void LightSetup::set_global_ambient_color(const Color& ambientColor) {
  mAmbientColor = ambientColor;
}

auto LightSetup::directional_lights() const -> const std::vector<
  DirectionalLight>& {
  return mDirectionalLights;
}

auto LightSetup::global_ambient_color() const -> Color {
  return mAmbientColor;
}

} // namespace basalt::gfx::backend
