#include <basalt/api/scene/transform_system.h>

#include <basalt/api/scene/ecs.h>
#include <basalt/api/scene/scene.h>
#include <basalt/api/scene/transform.h>

#include <basalt/api/math/matrix4x4.h>

namespace basalt {

namespace {

auto compute_child_local_to_world(EntityRegistry& entities,
                                  Matrix4x4f32 const& parentLocalToWorld,
                                  EntityId const childId) -> void {
  auto const& transform = entities.get<Transform const>(childId);
  auto& localToWorld = entities.get<LocalToWorld>(childId);
  localToWorld.matrix = transform.to_matrix() * parentLocalToWorld;

  // descent the hierarchy recursively
  if (auto const* children = entities.try_get<Children>(childId)) {
    for (auto const child : children->ids) {
      compute_child_local_to_world(entities, localToWorld.matrix, child);
    }
  }
}

} // namespace

auto TransformSystem::on_update(UpdateContext const& ctx) -> void {
  auto& entities = ctx.scene.entity_registry();

  auto const rootEntities = entities.view<Transform const, LocalToWorld>();

  rootEntities.each([](Transform const& transform, LocalToWorld& localToWorld) {
    localToWorld.matrix = transform.to_matrix();
  });

  // compute the LocalToWorld matrix for the children.
  // start with the parents at the root, then descent the hierarchy with a
  // recursive helper function
  auto const rootParents =
    entities.view<LocalToWorld const, Children const>(entt::exclude<Parent>);
  rootParents.each(
    [&](LocalToWorld const& localToWorld, Children const& children) {
      for (auto const child : children.ids) {
        compute_child_local_to_world(entities, localToWorld.matrix, child);
      }
    });
}

} // namespace basalt
