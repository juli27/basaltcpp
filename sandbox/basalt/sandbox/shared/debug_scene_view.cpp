#include "debug_scene_view.h"

#include "component_ui.h"

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/types.h>

#include <basalt/api/scene/ecs.h>
#include <basalt/api/scene/transform.h>

#include <entt/core/type_info.hpp>

#include <imgui.h>

#include <memory>
#include <string>
#include <utility>

using namespace std::literals;

using namespace basalt;

auto DebugSceneView::create(ScenePtr scene, gfx::ResourceCachePtr gfxCache,
                            EntityId const cameraEntity) -> DebugSceneViewPtr {
  return std::make_shared<DebugSceneView>(std::move(scene), std::move(gfxCache),
                                          cameraEntity);
}

DebugSceneView::DebugSceneView(ScenePtr scene, gfx::ResourceCachePtr gfxCache,
                               EntityId const cameraEntity)
  : SceneView{std::move(scene), std::move(gfxCache), cameraEntity} {
  mInspector.add_component_ui({
    entt::type_hash<Transform>::value(),
    "Transform"s,
    [](Entity const& entity) {
      ComponentUi::transform(entity.get<Transform>());
    },
  });
  mInspector.add_component_ui({
    entt::type_hash<LocalToWorld>::value(),
    "LocalToWorld"s,
    [](Entity const& entity) {
      ComponentUi::local_to_world(entity.get<LocalToWorld>());
    },
  });
  mInspector.add_component_ui({
    entt::type_hash<gfx::Camera>::value(),
    "gfx::Camera"s,
    [](Entity const& entity) {
      ComponentUi::camera(entity.get<gfx::Camera>());
    },
  });
  mInspector.add_component_ui({
    entt::type_hash<gfx::Model>::value(),
    "gfx::Model"s,
    [](Entity const& entity) {
      ComponentUi::model(entity.get<gfx::Model const>());
    },
  });
  mInspector.add_component_ui({
    entt::type_hash<gfx::Light>::value(),
    "gfx::Light"s,
    [](Entity const& entity) { ComponentUi::light(entity.get<gfx::Light>()); },
  });
  mInspector.add_component_ui({
    entt::type_hash<gfx::ext::XModel>::value(),
    "gfx::ext::XModel"s,
    [](Entity const& entity) {
      ComponentUi::x_model(entity.get<gfx::ext::XModel const>());
    },
  });
}

auto DebugSceneView::on_update(UpdateContext& ctx) -> void {
  SceneView::on_update(ctx);

  // append to view menu
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("View")) {
      ImGui::MenuItem("Scene Inspector", "I", &mInspectorOpen);

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }

  if (ImGui::Shortcut(ImGuiKey_I, ImGuiInputFlags_RouteGlobal)) {
    mInspectorOpen = !mInspectorOpen;
  }

  if (mInspectorOpen) {
    mInspector.show_window(*scene(), mInspectorOpen);
  }
}
