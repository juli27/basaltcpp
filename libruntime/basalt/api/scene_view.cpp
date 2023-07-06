#include <basalt/api/scene_view.h>

#include <basalt/api/debug_ui.h>
#include <basalt/api/engine.h>

#include <basalt/api/gfx/gfx_system.h>

#include <basalt/api/scene/ecs.h>
#include <basalt/api/scene/scene.h>

#include <basalt/api/shared/config.h>

#include <memory>
#include <utility>

namespace basalt {

using namespace std::literals;

using gfx::Camera;

auto SceneView::create(ScenePtr scene, const Camera& camera) -> SceneViewPtr {
  scene->create_system<gfx::GfxSystem>();

  return std::make_shared<SceneView>(std::move(scene), camera);
}

SceneView::SceneView(ScenePtr scene, const Camera& camera)
  : mScene {std::move(scene)}, mCamera {camera} {
}

auto SceneView::camera() const noexcept -> const Camera& {
  return mCamera;
}

auto SceneView::on_update(UpdateContext& ctx) -> void {
  Engine& engine {ctx.engine};

  auto& entityRegistry {mScene->entity_registry()};
  entityRegistry.ctx().insert_or_assign(&ctx.drawCtx);
  if (!entityRegistry.ctx().contains<gfx::ResourceCache*>()) {
    entityRegistry.ctx().insert_or_assign(&engine.gfx_resource_cache());
  }
  if (!entityRegistry.ctx().contains<Camera*>()) {
    entityRegistry.ctx().insert_or_assign(&mCamera);
  }

  const Scene::UpdateContext sceneCtx {engine.delta_time()};
  mScene->on_update(sceneCtx);

  auto& config {engine.config()};

  if (bool sceneInspectorEnabled {
        config.get_bool("debug.scene_inspector.enabled"s)}) {
    DebugUi::show_scene_inspector(*mScene, sceneInspectorEnabled);

    config.set_bool("debug.scene_inspector.enabled"s, sceneInspectorEnabled);
  }
}

} // namespace basalt
