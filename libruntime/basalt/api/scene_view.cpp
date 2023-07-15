#include <basalt/api/scene_view.h>

#include <basalt/api/debug_ui.h>
#include <basalt/api/engine.h>
#include <basalt/api/input.h>

#include <basalt/api/gfx/gfx_system.h>
#include <basalt/api/gfx/resource_cache.h>

#include <basalt/api/scene/ecs.h>
#include <basalt/api/scene/scene.h>

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
  : mScene {std::move(scene)}, mGfxCache {std::move(gfxCache)} {
  auto& ctx {mScene->entity_registry().ctx()};
  ctx.emplace<const InputState&>(input_state());
  ctx.emplace<gfx::ResourceCache&>(*mGfxCache);
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

  auto& config {engine.config()};

  if (bool sceneInspectorEnabled {
        config.get_bool("debug.scene_inspector.enabled"s)}) {
    DebugUi::show_scene_inspector(*mScene, sceneInspectorEnabled);

    config.set_bool("debug.scene_inspector.enabled"s, sceneInspectorEnabled);
  }
}

} // namespace basalt
