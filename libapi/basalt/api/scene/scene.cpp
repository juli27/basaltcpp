#include <basalt/api/scene/scene.h>

#include <basalt/api/scene/transform.h>

#include <entt/entity/handle.hpp>

using std::vector;

using entt::entity;
using entt::registry;

namespace basalt {

auto Scene::ecs() -> registry& {
  return mEntityRegistry;
}

auto Scene::create_entity() -> entt::handle {
  const entity id = mEntityRegistry.create();
  mEntityRegistry.emplace<Transform>(id);

  return entt::handle {mEntityRegistry, id};
}

auto Scene::get_handle(const entity entity) -> entt::handle {
  return entt::handle {mEntityRegistry, entity};
}

auto Scene::background() const -> const Color& {
  return mBackgroundColor;
}

void Scene::set_background(const Color& background) {
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
void Scene::add_directional_light(const Vector3f32& direction,
                                  const Color& color) {
  mDirectionalLights.emplace_back(
    DirectionalLight {direction, color, Color {}});
}

void Scene::clear_directional_lights() {
  mDirectionalLights.clear();
}

} // namespace basalt
