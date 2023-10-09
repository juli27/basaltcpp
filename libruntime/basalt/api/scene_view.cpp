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
                       EntityId const cameraEntity) -> SceneViewPtr {
  scene->create_system<gfx::GfxSystem>();

  auto& ctx = scene->entity_registry().ctx();
  ctx.emplace_as<EntityId>(gfx::GfxSystem::sMainCamera, cameraEntity);

  return std::make_shared<SceneView>(std::move(scene), std::move(gfxCache));
}

SceneView::SceneView(ScenePtr scene, gfx::ResourceCachePtr gfxCache)
  : mScene{std::move(scene)}
  , mGfxCache{std::move(gfxCache)}
  , mSelectedEntity{entt::null} {
  auto& ctx = mScene->entity_registry().ctx();
  ctx.emplace<InputState const&>(input_state());
  ctx.emplace<gfx::ResourceCache&>(*mGfxCache);

  mComponentUis.push_back({
    entt::type_hash<Transform>::value(),
    "Transform"s,
    [](Entity const& entity) { DebugUi::transform(entity.get<Transform>()); },
  });
  mComponentUis.push_back({
    entt::type_hash<LocalToWorld>::value(),
    "LocalToWorld"s,
    [](Entity const& entity) {
      DebugUi::local_to_world(entity.get<LocalToWorld>());
    },
  });
  mComponentUis.push_back({
    entt::type_hash<gfx::Camera>::value(),
    "gfx::Camera"s,
    [](Entity const& entity) { DebugUi::camera(entity.get<gfx::Camera>()); },
  });
  mComponentUis.push_back({
    entt::type_hash<gfx::RenderComponent>::value(),
    "gfx::RenderComponent"s,
    [](Entity const& entity) {
      DebugUi::render_component(entity.get<gfx::RenderComponent const>());
    },
  });
  mComponentUis.push_back({
    entt::type_hash<gfx::Light>::value(),
    "gfx::Light"s,
    [](Entity const& entity) { DebugUi::light(entity.get<gfx::Light>()); },
  });
  mComponentUis.push_back({
    entt::type_hash<gfx::ext::XModel>::value(),
    "gfx::ext::XModel"s,
    [](Entity const& entity) {
      DebugUi::x_model(entity.get<gfx::ext::XModel const>());
    },
  });
}

auto SceneView::update_debug_ui(Config& config) -> void {
  if (auto sceneInspectorOpen =
        config.get_bool("debug.scene_inspector.visible"s)) {
    auto const& state = mScene->entity_registry().ctx().insert_or_assign(
      DebugUi::SceneInspectorState{mSelectedEntity, mComponentUis});
    DebugUi::scene_inspector(*mScene, sceneInspectorOpen);
    mSelectedEntity = state.selected;

    config.set_bool("debug.scene_inspector.visible"s, sceneInspectorOpen);
  }
}

auto SceneView::on_input(InputEvent const&) -> InputEventHandled {
  return InputEventHandled::Yes;
}

auto SceneView::on_update(UpdateContext& ctx) -> void {
  auto& engine = ctx.engine;

  auto& ecsCtx = mScene->entity_registry().ctx();
  ecsCtx.insert_or_assign<DrawContext const&>(ctx.drawCtx);

  auto const sceneCtx = Scene::UpdateContext{ctx.deltaTime};
  mScene->on_update(sceneCtx);

  if (auto& config = engine.config();
      config.get_bool("runtime.debugUI.enabled"s)) {
    update_debug_ui(config);
  }
}

} // namespace basalt
