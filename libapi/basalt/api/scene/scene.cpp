#include <basalt/api/scene/scene.h>

#include <basalt/api/scene/transform.h>

#include <entt/entity/handle.hpp>

#include <memory>

using std::vector;

using entt::entity;
using entt::registry;

namespace basalt {

using gfx::DirectionalLight;

auto Scene::create() -> ScenePtr {
  return std::make_shared<Scene>();
}

auto Scene::ecs() -> registry& {
  return mEntityRegistry;
}

auto Scene::create_entity(const Vector3f32& position,
                          const Vector3f32& rotation, const Vector3f32& scale)
  -> entt::handle {
  const entity id = mEntityRegistry.create();
  mEntityRegistry.emplace<Transform>(id, position, rotation, scale);

  return entt::handle {mEntityRegistry, id};
}

auto Scene::get_handle(const entity entity) -> entt::handle {
  return entt::handle {mEntityRegistry, entity};
}

auto Scene::destroy_system(const SystemId id) -> void {
  mSystems.deallocate(id);
}

auto Scene::on_update(const SceneContext& ctx) -> void {
  for (const auto& [handle, system] : mSystems) {
    const SystemContext context {ctx.deltaTimeSeconds, *this};

    (*system)->on_update(context);
  }
}

auto Scene::background() const -> const Color& {
  return mBackgroundColor;
}

auto Scene::set_background(const Color& background) -> void {
  mBackgroundColor = background;
}

auto Scene::set_ambient_light(const Color& color) -> void {
  mAmbientLightColor = color;
}

auto Scene::ambient_light() const -> const Color& {
  return mAmbientLightColor;
}

auto Scene::directional_lights() const -> const vector<DirectionalLight>& {
  return mDirectionalLights;
}

// TODO: ambient color support
auto Scene::add_directional_light(const Vector3f32& direction,
                                  const Color& color) -> void {
  mDirectionalLights.emplace_back(
    DirectionalLight {direction, color, Color {}});
}

auto Scene::clear_directional_lights() -> void {
  mDirectionalLights.clear();
}

} // namespace basalt
