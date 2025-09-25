#include <basalt/api/scene_view.h>

#include <basalt/api/input.h>

#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/environment.h>
#include <basalt/api/gfx/gfx_system.h>
#include <basalt/api/gfx/resource_cache.h>

#include <basalt/api/scene/ecs.h>
#include <basalt/api/scene/scene.h>

#include <memory>
#include <utility>

namespace basalt {

// TODO: make gfxCache somehow bound to the GfxSystem

auto SceneView::create(ScenePtr scene, gfx::ResourceCachePtr gfxCache,
                       EntityId const cameraEntity) -> SceneViewPtr {
  return std::make_shared<SceneView>(std::move(scene), std::move(gfxCache),
                                     cameraEntity);
}

SceneView::SceneView(ScenePtr scene, gfx::ResourceCachePtr gfxCache,
                     EntityId cameraEntity)
  : mScene{std::move(scene)}
  , mGfxCache{std::move(gfxCache)} {
  mScene->create_system<gfx::GfxSystem>();

  auto& ctx = mScene->entity_registry().ctx();
  ctx.emplace<InputState const&>(input_state());
  ctx.emplace<gfx::Environment>();
  ctx.emplace<gfx::ResourceCache&>(*mGfxCache);
  ctx.emplace<gfx::Context&>(*mGfxCache->context());
  ctx.emplace_as<EntityId>(gfx::GfxSystem::sMainCamera, cameraEntity);
}

auto SceneView::scene() const -> ScenePtr const& {
  return mScene;
}

auto SceneView::on_input(InputEvent const&) -> InputEventHandled {
  return InputEventHandled::Yes;
}

auto SceneView::on_update(UpdateContext& ctx) -> void {
  auto& ecsCtx = mScene->entity_registry().ctx();
  ecsCtx.insert_or_assign<DrawContext const&>(ctx.drawCtx);

  auto const sceneCtx = Scene::UpdateContext{ctx.deltaTime};
  mScene->on_update(sceneCtx);
  ecsCtx.erase<DrawContext const&>();
}

} // namespace basalt
