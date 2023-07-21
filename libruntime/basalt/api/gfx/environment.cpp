#include <basalt/api/gfx/environment.h>

namespace basalt::gfx {

auto Environment::background() const noexcept -> const Color& {
  return mBackground;
}

auto Environment::set_background(const Color& background) noexcept -> void {
  mBackground = background;
}

auto Environment::ambient_light() const noexcept -> const Color& {
  return mAmbientLight;
}

auto Environment::set_ambient_light(const Color& ambientLight) noexcept
  -> void {
  mAmbientLight = ambientLight;
}

auto Environment::directional_lights() const noexcept
  -> gsl::span<const DirectionalLight> {
  return mDirectionalLights;
}

auto Environment::add_directional_light(const Vector3f32& direction,
                                        const Color& diffuse,
                                        const Color& specular,
                                        const Color& ambient) -> void {
  mDirectionalLights.push_back(
    DirectionalLight {diffuse, specular, ambient, direction});
}

auto Environment::clear_directional_lights() noexcept -> void {
  mDirectionalLights.clear();
}

} // namespace basalt::gfx
