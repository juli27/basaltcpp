#include <basalt/api/scene/parent_system.h>

#include <basalt/api/scene/ecs.h>
#include <basalt/api/scene/scene.h>
#include <basalt/api/scene/transform.h>

namespace basalt {

auto ParentSystem::on_update(UpdateContext const& ctx) -> void {
  auto& entityRegistry = ctx.scene.entity_registry();

  entityRegistry.clear<Children>();

  auto const children = entityRegistry.view<Parent>();

  for (auto const&& [id, parent] : children.each()) {
    auto& childrenOfParent = entityRegistry.get_or_emplace<Children>(parent.id);
    childrenOfParent.ids.push_back(id);
  }
}

} // namespace basalt
