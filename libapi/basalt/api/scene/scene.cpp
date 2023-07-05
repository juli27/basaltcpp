#include <basalt/api/scene/scene.h>

#include <basalt/api/scene/transform.h>

#include <memory>

using std::vector;

namespace basalt {

using gfx::DirectionalLight;

auto Scene::create() -> ScenePtr {
  return std::make_shared<Scene>();
}

auto Scene::entity_registry() -> EntityRegistry& {
  return mEntityRegistry;
}

auto Scene::create_entity(const Vector3f32& position,
                          const Vector3f32& rotation, const Vector3f32& scale)
  -> Entity {
  const EntityId id {mEntityRegistry.create()};
  mEntityRegistry.emplace<Transform>(id, position, rotation, scale);

  return Entity {mEntityRegistry, id};
}

auto Scene::get_handle(const EntityId entity) -> Entity {
  return Entity {mEntityRegistry, entity};
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
