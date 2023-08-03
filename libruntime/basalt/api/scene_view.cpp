#include <basalt/api/scene_view.h>

#include <basalt/api/debug_ui.h>
#include <basalt/api/engine.h>
#include <basalt/api/input.h>

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/gfx_system.h>
#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/scene/ecs.h>
#include <basalt/api/scene/scene.h>
#include <basalt/api/scene/transform.h>

#include <basalt/api/shared/config.h>

#include <entt/core/hashed_string.hpp>

#include <memory>
#include <utility>

namespace basalt {

using namespace std::literals;

using namespace entt::literals;

// TODO: make gfxCache somehow bound to the GfxSystem

auto SceneView::create(ScenePtr scene, gfx::ResourceCachePtr gfxCache,
                       const EntityId cameraEntity) -> SceneViewPtr {
  scene->create_system<gfx::GfxSystem>();

  auto& ctx {scene->entity_registry().ctx()};
  ctx.emplace_as<EntityId>(gfx::GfxSystem::sMainCamera, cameraEntity);

  return std::make_shared<SceneView>(std::move(scene), std::move(gfxCache));
}

SceneView::SceneView(ScenePtr scene, gfx::ResourceCachePtr gfxCache)
  : mScene {std::move(scene)}
  , mGfxCache {std::move(gfxCache)}
  , mSelectedEntity {entt::null} {
  auto& ctx {mScene->entity_registry().ctx()};
  ctx.emplace<const InputState&>(input_state());
  ctx.emplace<gfx::ResourceCache&>(*mGfxCache);

  mComponentUis.push_back({
    entt::type_hash<Transform>::value(),
    "Transform"s,
    [](const Entity& entity) { DebugUi::transform(entity.get<Transform>()); },
  });
  mComponentUis.push_back({
    entt::type_hash<LocalToWorld>::value(),
    "LocalToWorld"s,
    [](const Entity& entity) {
      DebugUi::local_to_world(entity.get<LocalToWorld>());
    },
  });
  mComponentUis.push_back({
    entt::type_hash<gfx::Camera>::value(),
    "gfx::Camera"s,
    [](const Entity& entity) { DebugUi::camera(entity.get<gfx::Camera>()); },
  });
  mComponentUis.push_back({
    entt::type_hash<gfx::RenderComponent>::value(),
    "gfx::RenderComponent"s,
    [](const Entity& entity) {
      DebugUi::render_component(entity.get<const gfx::RenderComponent>());
    },
  });
  mComponentUis.push_back({
    entt::type_hash<gfx::Light>::value(),
    "gfx::Light"s,
    [](const Entity& entity) { DebugUi::light(entity.get<gfx::Light>()); },
  });
  mComponentUis.push_back({
    entt::type_hash<gfx::ext::XModel>::value(),
    "gfx::ext::XModel"s,
    [](const Entity& entity) {
      DebugUi::x_model(entity.get<const gfx::ext::XModel>());
    },
  });
}

auto SceneView::update_debug_ui(Config& config) -> void {
  if (bool sceneInspectorOpen {
        config.get_bool("debug.scene_inspector.visible"s)}) {
    const auto& state {mScene->entity_registry().ctx().insert_or_assign(
      DebugUi::SceneInspectorState {mSelectedEntity, mComponentUis})};
    DebugUi::scene_inspector(*mScene, sceneInspectorOpen);
    mSelectedEntity = state.selected;

    config.set_bool("debug.scene_inspector.visible"s, sceneInspectorOpen);
  }
}

auto SceneView::on_input(const InputEvent&) -> InputEventHandled {
  return InputEventHandled::Yes;
}

auto SceneView::on_update(UpdateContext& ctx) -> void {
  Engine& engine {ctx.engine};

  auto& ecsCtx {mScene->entity_registry().ctx()};
  ecsCtx.insert_or_assign<const DrawContext&>(ctx.drawCtx);

  const Scene::UpdateContext sceneCtx {ctx.deltaTime};
  mScene->on_update(sceneCtx);

  if (auto& config {engine.config()};
      config.get_bool("runtime.debugUI.enabled"s)) {
    update_debug_ui(config);
  }
}

} // namespace basalt
