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
  ImGui::SetNextWindowSize(ImVec2{600.0f, 600.0f}, ImGuiCond_FirstUseEver);
  if (!ImGui::Begin("Scene Inspector", &open)) {
    ImGui::End();
    return;
  }

  auto& entities = scene.entity_registry();

  constexpr auto windowFlags = ImGuiWindowFlags_HorizontalScrollbar;

  if (ImGui::BeginChild(
        "entities", ImVec2{0.33f * ImGui::GetContentRegionAvail().x, 0},
        ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX, windowFlags)) {
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

auto SceneInspector::entity_hierarchy(EntityRegistry& entities) -> void {
  auto const rootEntities = entities.view<EntityId>(entt::exclude<Parent>);

  for (auto const id : rootEntities) {
    entity_node(Entity{entities, id});
  }
}

auto SceneInspector::entity_node(Entity entity) -> void {
  constexpr auto baseFlags =
    ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow |
    ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_NavLeftJumpsToParent;
  constexpr auto leafFlags =
    baseFlags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

  auto const* children = entity.try_get<Children>();
  auto const hasChildren = children != nullptr;
  auto flags = hasChildren ? baseFlags : leafFlags;
  if (mSelectedEntity == entity) {
    flags |= ImGuiTreeNodeFlags_Selected;
  }

  auto const entityId = to_integral(entity.entity());
  ImGui::PushID(entityId);

  auto const open = [&] {
    if (auto const* name{entity.try_get<EntityName const>()}) {
      return ImGui::TreeNodeEx(name->value.c_str(), flags);
    }

    return ImGui::TreeNodeEx("", flags, "Entity %d", entityId);
  }();

  if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
    mSelectedEntity = entity;
  }

  if (open && children) {
    // recursively traverse children
    for (auto const childId : children->ids) {
      entity_node(Entity{*entity.registry(), childId});
    }

    ImGui::TreePop();
  }

  ImGui::PopID();
}

// TODO: allow skipping over components
auto SceneInspector::entity_components(EntityRegistry& entities) -> void {
  if (mSelectedEntity == entt::null) {
    ImGui::TextUnformatted("no entity selected");

    return;
  }

  constexpr auto nodeFlags = ImGuiTreeNodeFlags_DefaultOpen;

  for (auto const& componentUi : mComponentUis) {
    auto const* storage = entities.storage(componentUi.typeId);
    if (!storage) {
      continue;
    }

    if (!storage->contains(mSelectedEntity)) {
      continue;
    }

    if (ImGui::CollapsingHeader(componentUi.name.c_str(), nodeFlags)) {
      componentUi.render(Entity{entities, mSelectedEntity});
    }
  }

  for (auto const [id, storage] : entities.storage()) {
    if (!storage.contains(mSelectedEntity)) {
      continue;
    }

    // skip components that have a custom UI
    auto const it =
      std::find_if(mComponentUis.cbegin(), mComponentUis.cend(),
                   [&](ComponentUi const& ui) { return id == ui.typeId; });
    if (it != mComponentUis.cend()) {
      continue;
    }

    auto nameView = storage.info().name();
    // remove "class " / "struct " prefix
    auto pos = nameView.find_first_of(' ');
    if (pos != decltype(nameView)::npos) {
      nameView.remove_prefix(pos);
    }

    auto name = std::string{nameView};
    if (ImGui::CollapsingHeader(name.c_str())) {
      ImGui::TextUnformatted("no component ui registered");
    }
  }
}
