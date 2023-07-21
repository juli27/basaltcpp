#include <basalt/api/scene/parent_system.h>

#include <basalt/api/scene/ecs.h>
#include <basalt/api/scene/scene.h>
#include <basalt/api/scene/transform.h>
#include <basalt/api/scene/types.h>

namespace basalt {

auto ParentSystem::on_update(const UpdateContext& ctx) -> void {
  EntityRegistry& entityRegistry {ctx.scene.entity_registry()};

  entityRegistry.clear<Children>();

  const auto children {entityRegistry.view<Parent>()};

  for (const auto&& [id, parent] : children.each()) {
    Children& childrenOfParent {
      entityRegistry.get_or_emplace<Children>(parent.id)};
    childrenOfParent.ids.push_back(id);
  }
}

} // namespace basalt
