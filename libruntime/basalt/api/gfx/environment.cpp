#include <basalt/api/gfx/environment.h>

namespace basalt::gfx {

auto Environment::background() const noexcept -> Color const& {
  return mBackground;
}

auto Environment::set_background(Color const& background) noexcept -> void {
  mBackground = background;
}

auto Environment::ambient_light() const noexcept -> Color const& {
  return mAmbientLight;
}

auto Environment::set_ambient_light(Color const& ambientLight) noexcept
  -> void {
  mAmbientLight = ambientLight;
}

auto Environment::directional_lights() const noexcept
  -> gsl::span<DirectionalLight const> {
  return mDirectionalLights;
}

auto Environment::add_directional_light(Vector3f32 const& direction,
                                        Color const& diffuse,
                                        Color const& specular,
                                        Color const& ambient) -> void {
  mDirectionalLights.push_back(
    DirectionalLight{diffuse, specular, ambient, direction});
}

auto Environment::clear_directional_lights() noexcept -> void {
  mDirectionalLights.clear();
}

} // namespace basalt::gfx
