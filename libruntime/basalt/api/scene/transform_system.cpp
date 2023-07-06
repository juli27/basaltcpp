#include <basalt/api/scene/transform_system.h>

#include <basalt/api/scene/ecs.h>
#include <basalt/api/scene/scene.h>
#include <basalt/api/scene/transform.h>

#include <basalt/api/math/matrix4x4.h>

namespace basalt {

auto TransformSystem::on_update(const UpdateContext& ctx) -> void {
  EntityRegistry& entityRegistry {ctx.scene.entity_registry()};

  entityRegistry.view<const Transform>().each(
    [&](const EntityId eId, const Transform& transform) {
      const auto localToWorld {Matrix4x4f32::scaling(transform.scale) *
                               Matrix4x4f32::rotation(transform.rotation) *
                               Matrix4x4f32::translation(transform.position)};

      entityRegistry.emplace_or_replace<LocalToWorld>(eId, localToWorld);
    });
}

} // namespace basalt
