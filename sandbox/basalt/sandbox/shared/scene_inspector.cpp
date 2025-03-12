#include "scene_inspector.h"

#include <basalt/api/debug_ui.h>

#include <basalt/api/scene/ecs.h>
#include <basalt/api/scene/scene.h>
#include <basalt/api/scene/transform.h>

#include <imgui.h>

#include <utility>

using namespace basalt;

SceneInspector::SceneInspector() : mSelectedEntity{entt::null} {
}

auto SceneInspector::add_component_ui(ComponentUi ui) -> void {
  mComponentUis.push_back(std::move(ui));
}

// TODO: show systems and allow to enable/disable them
// TODO: how to display system state like main camera and gfx resource cache
auto SceneInspector::show_window(Scene& scene, bool& open) -> void {
  ImGui::SetNextWindowSize(ImVec2{400.0f, 600.0f}, ImGuiCond_FirstUseEver);
  if (!ImGui::Begin("Scene Inspector", &open)) {
    ImGui::End();
    return;
  }

  auto& entities = scene.entity_registry();

  constexpr auto windowFlags = ImGuiWindowFlags_HorizontalScrollbar;

  if (ImGui::BeginChild("hierarchy",
                        ImVec2{ImGui::GetContentRegionAvail().x * 0.25f, 0}, 0,
                        windowFlags)) {
    entity_hierarchy(entities);
  }
  ImGui::EndChild();

  ImGui::SameLine();

  if (ImGui::BeginChild("components", ImVec2{}, 0, windowFlags)) {
    entity_components(entities);
  }
  ImGui::EndChild();

  ImGui::End();
}

namespace {

auto entity_node(Entity const entity, EntityId& selected) -> void {
  constexpr auto baseNodeFlags = ImGuiTreeNodeFlags_OpenOnDoubleClick |
                                 ImGuiTreeNodeFlags_OpenOnArrow |
                                 ImGuiTreeNodeFlags_SpanAvailWidth;
  constexpr auto leafNodeFlags = baseNodeFlags | ImGuiTreeNodeFlags_Leaf;
  ImGui::PushID(to_integral(entity.entity()));

  auto const* children = entity.try_get<Children>();
  auto const isParent = children != nullptr;
  auto nodeFlags = isParent ? baseNodeFlags : leafNodeFlags;

  if (selected == entity) {
    nodeFlags |= ImGuiTreeNodeFlags_Selected;
  }

  auto const entityNode = [](Entity const e,
                             ImGuiTreeNodeFlags const flags) -> bool {
    if (auto const* name{e.try_get<EntityName const>()}) {
      return ImGui::TreeNodeEx(name->value.c_str(), flags);
    }

    return ImGui::TreeNodeEx("Entity", flags, "Entity %d",
                             to_integral(e.entity()));
  };

  auto const open = entityNode(entity, nodeFlags);

  if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
    selected = entity;
  }

  if (open) {
    if (children) {
      for (auto const childId : children->ids) {
        entity_node(Entity{*entity.registry(), childId}, selected);
      }
    }

    ImGui::TreePop();
  }

  ImGui::PopID();
}

} // namespace

auto SceneInspector::entity_hierarchy(EntityRegistry& entities) -> void {
  auto const rootEntities = entities.view<EntityId>(entt::exclude<Parent>);

  for (auto const id : rootEntities) {
    entity_node(Entity{entities, id}, mSelectedEntity);
  }
}

auto SceneInspector::entity_components(EntityRegistry& entities) -> void {
  if (mSelectedEntity == entt::null) {
    ImGui::TextUnformatted("no entity selected");

    return;
  }

  constexpr auto nodeFlags =
    ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth;

  // TODO: add a way to show components which don't have a UI
  for (auto const& componentUi : mComponentUis) {
    auto const* storage = entities.storage(componentUi.typeId);
    if (!storage) {
      continue;
    }

    if (!storage->contains(mSelectedEntity)) {
      continue;
    }

    if (ImGui::TreeNodeEx(componentUi.name.c_str(), nodeFlags)) {
      componentUi.render(Entity{entities, mSelectedEntity});

      ImGui::TreePop();
    }
  }
}
