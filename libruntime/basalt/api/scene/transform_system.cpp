#include <basalt/api/scene/transform_system.h>

#include <basalt/api/scene/ecs.h>
#include <basalt/api/scene/scene.h>
#include <basalt/api/scene/transform.h>

#include <basalt/api/math/matrix4x4.h>

namespace basalt {

namespace {

auto compute_child_local_to_world(EntityRegistry& entities,
                                  const Matrix4x4f32& parentLocalToWorld,
                                  const EntityId childId) -> void {
  const auto& transform {entities.get<const Transform>(childId)};
  auto& localToWorld {entities.get<LocalToWorld>(childId)};
  localToWorld.matrix = transform.to_matrix() * parentLocalToWorld;

  // descent the hierarchy recursively
  if (const Children * children {entities.try_get<Children>(childId)}) {
    for (const EntityId child : children->ids) {
      compute_child_local_to_world(entities, localToWorld.matrix, child);
    }
  }
}

} // namespace

auto TransformSystem::on_update(const UpdateContext& ctx) -> void {
  EntityRegistry& entities {ctx.scene.entity_registry()};

  const auto rootEntities {entities.view<const Transform, LocalToWorld>()};

  rootEntities.each([](const Transform& transform, LocalToWorld& localToWorld) {
    localToWorld.matrix = transform.to_matrix();
  });

  // compute the LocalToWorld matrix for the children.
  // start with the parents at the root, then descent the hierarchy with a
  // recursive helper function
  const auto rootParents {
    entities.view<const LocalToWorld, const Children>(entt::exclude<Parent>)};
  rootParents.each(
    [&](const LocalToWorld& localToWorld, const Children& children) {
      for (const EntityId child : children.ids) {
        compute_child_local_to_world(entities, localToWorld.matrix, child);
      }
    });
}

} // namespace basalt
