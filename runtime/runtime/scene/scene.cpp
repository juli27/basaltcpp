#include "scene.h"

using std::vector;

using entt::registry;

namespace basalt {

auto Scene::ecs() -> registry& {
  return mEntityRegistry;
}

auto Scene::background_color() const -> const Color& {
  return mBackgroundColor;
}

void Scene::set_background_color(const Color& background) {
  mBackgroundColor = background;
}

void Scene::set_ambient_light(const Color& color) {
  mAmbientLightColor = color;
}

auto Scene::ambient_light() const -> const Color& {
  return mAmbientLightColor;
}

auto Scene::directional_lights() const -> const vector<DirectionalLight>& {
  return mDirectionalLights;
}

// TODO: ambient color support
void Scene::add_directional_light(
  const Vec3f32& direction, const Color& color) {
  mDirectionalLights.push_back(DirectionalLight {direction, color, Color {}});
}

void Scene::clear_directional_lights() {
  mDirectionalLights.clear();
}

} // namespace basalt
