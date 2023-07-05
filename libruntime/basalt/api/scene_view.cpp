#include <basalt/api/scene_view.h>

#include <basalt/gfx/filtering_command_list.h>

#include <basalt/api/debug_ui.h>
#include <basalt/api/engine.h>

#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/types.h>

#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <basalt/api/scene/ecs.h>
#include <basalt/api/scene/scene.h>
#include <basalt/api/scene/transform.h>

#include <basalt/api/shared/config.h>

#include <memory>
#include <utility>
#include <vector>

namespace basalt {

using namespace std::literals;
using std::vector;

using gfx::Attachment;
using gfx::Attachments;
using gfx::Camera;
using gfx::FilteringCommandList;
using gfx::Light;
using gfx::MaterialData;
using gfx::RenderComponent;
using gfx::TransformState;

namespace ext = gfx::ext;

namespace {

auto record_material(FilteringCommandList& cmdList, const MaterialData& data)
  -> void {
  cmdList.bind_pipeline(data.pipeline);
  cmdList.bind_texture(data.texture);
  cmdList.bind_sampler(data.sampler);

  cmdList.set_material(data.diffuse, data.ambient, Color {});
}

} // namespace

auto SceneView::create(ScenePtr scene, const Camera& camera) -> SceneViewPtr {
  return std::make_shared<SceneView>(std::move(scene), camera);
}

SceneView::SceneView(ScenePtr scene, const Camera& camera)
  : mScene {std::move(scene)}, mCamera {camera} {
}

auto SceneView::camera() const noexcept -> const Camera& {
  return mCamera;
}

auto SceneView::on_draw(const DrawContext& context) -> void {
  FilteringCommandList cmdList {};
  cmdList.clear_attachments(
    Attachments {Attachment::RenderTarget, Attachment::DepthBuffer},
    mScene->background(), 1.0f, 0);

  cmdList.set_transform(TransformState::ViewToViewport,
                        mCamera.view_to_viewport(context.viewport));
  cmdList.set_transform(TransformState::WorldToView, mCamera.world_to_view());

  cmdList.set_ambient_light(mScene->ambient_light());

  const auto& directionalLights {mScene->directional_lights()};
  if (!directionalLights.empty()) {
    const vector<Light> lights {directionalLights.begin(),
                                directionalLights.end()};
    cmdList.set_lights(lights);
  }

  const auto& cache {context.cache};
  const auto& ecs {mScene->entity_registry()};

  ecs.view<const LocalToWorld, const ext::XModel>().each(
    [&](const LocalToWorld& localToWorld, const ext::XModel& model) {
      cmdList.set_transform(TransformState::ModelToWorld, localToWorld.value);

      const auto& modelData {cache.get(model)};

      const auto numMaterials {static_cast<u32>(modelData.materials.size())};

      for (u32 i {0}; i < numMaterials; ++i) {
        const auto& materialData {cache.get(modelData.materials[i])};
        record_material(cmdList, materialData);

        ext::XMeshCommandEncoder::draw_x_mesh(cmdList.cmd_list(),
                                              modelData.mesh, i);
      }
    });

  ecs.view<const LocalToWorld, const RenderComponent>().each(
    [&](const LocalToWorld& localToWorld,
        const RenderComponent& renderComponent) {
      const MaterialData& materialData {cache.get(renderComponent.material)};
      record_material(cmdList, materialData);

      cmdList.set_transform(TransformState::Texture,
                            renderComponent.texTransform);
      cmdList.set_transform(TransformState::ModelToWorld, localToWorld.value);

      const auto& meshData = cache.get(renderComponent.mesh);
      cmdList.bind_vertex_buffer(meshData.vertexBuffer, 0ull);
      cmdList.draw(meshData.startVertex, meshData.vertexCount);
    });

  context.commandLists.push_back(cmdList.take_cmd_list());
}

auto SceneView::on_tick(Engine& engine) -> void {
  const SceneContext sceneCtx {engine.delta_time()};
  mScene->on_update(sceneCtx);

  auto& config {engine.config()};

  if (bool sceneInspectorEnabled {
        config.get_bool("debug.scene_inspector.enabled"s)}) {
    DebugUi::show_scene_inspector(*mScene, sceneInspectorEnabled);

    config.set_bool("debug.scene_inspector.enabled"s, sceneInspectorEnabled);
  }
}

} // namespace basalt
