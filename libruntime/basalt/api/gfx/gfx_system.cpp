#include <basalt/api/gfx/gfx_system.h>

#include <basalt/gfx/filtering_command_list.h>

#include <basalt/api/view.h> // for DrawContext ...

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <basalt/api/scene/ecs.h>
#include <basalt/api/scene/scene.h>
#include <basalt/api/scene/transform.h>
#include <basalt/api/scene/types.h>

#include <vector>

using std::vector;

namespace basalt::gfx {

namespace {

auto record_camera(FilteringCommandList& cmdList, Scene& scene,
                   const f32 aspectRatio) -> void {
  EntityRegistry& entities {scene.entity_registry()};
  const CameraEntity cameraEntity {
    scene.get_handle(entities.ctx().get<EntityId>(GfxSystem::sMainCamera))};

  cameraEntity.get_camera().aspectRatio = aspectRatio;

  cmdList.set_transform(TransformState::ViewToClip,
                        cameraEntity.view_to_clip());
  cmdList.set_transform(TransformState::WorldToView,
                        cameraEntity.world_to_view());
}

auto record_material(FilteringCommandList& cmdList, const ResourceCache& cache,
                     const Material id) -> void {
  const MaterialData& data {cache.get(id)};

  cmdList.bind_pipeline(data.pipeline);
  cmdList.bind_texture(data.texture);
  cmdList.bind_sampler(data.sampler);

  cmdList.set_material(data.diffuse, data.ambient);
}

auto record_render_component(FilteringCommandList& cmdList,
                             const ResourceCache& cache,
                             const LocalToWorld& localToWorld,
                             const RenderComponent& renderComponent) {
  record_material(cmdList, cache, renderComponent.material);

  cmdList.set_transform(TransformState::Texture, renderComponent.texTransform);
  cmdList.set_transform(TransformState::LocalToWorld, localToWorld.matrix);

  const auto& meshData = cache.get(renderComponent.mesh);
  cmdList.bind_vertex_buffer(meshData.vertexBuffer);

  if (meshData.indexBuffer) {
    cmdList.bind_index_buffer(meshData.indexBuffer);
    cmdList.draw_indexed(0, 0, meshData.vertexCount, 0, meshData.indexCount);
  } else {
    cmdList.draw(meshData.startVertex, meshData.vertexCount);
  }
}

} // namespace

auto GfxSystem::on_update(const UpdateContext& ctx) -> void {
  Scene& scene {ctx.scene};
  EntityRegistry& entities {scene.entity_registry()};
  const auto& ecsCtx {entities.ctx()};

  const auto& drawCtx {ecsCtx.get<const View::DrawContext>()};
  const auto& cache {ecsCtx.get<const ResourceCache>()};

  FilteringCommandList cmdList;
  cmdList.clear_attachments(
    Attachments {Attachment::RenderTarget, Attachment::DepthBuffer},
    scene.background(), 1.0f);

  const f32 aspectRatio {drawCtx.viewport.aspect_ratio()};
  record_camera(cmdList, scene, aspectRatio);

  cmdList.set_ambient_light(scene.ambient_light());

  const auto& directionalLights {scene.directional_lights()};
  if (!directionalLights.empty()) {
    const vector<Light> lights {directionalLights.begin(),
                                directionalLights.end()};
    cmdList.set_lights(lights);
  }

  entities.view<const LocalToWorld, const ext::XModel>().each(
    [&](const LocalToWorld& localToWorld, const ext::XModel& model) {
      cmdList.set_transform(TransformState::LocalToWorld, localToWorld.matrix);

      const auto& modelData {cache.get(model)};

      const auto numMaterials {static_cast<u32>(modelData.materials.size())};

      for (u32 i {0}; i < numMaterials; ++i) {
        record_material(cmdList, cache, modelData.materials[i]);

        ext::XMeshCommandEncoder::draw_x_mesh(cmdList.cmd_list(),
                                              modelData.mesh, i);
      }
    });

  entities.view<const LocalToWorld, const RenderComponent>().each(
    [&](const LocalToWorld& localToWorld,
        const RenderComponent& renderComponent) {
      record_render_component(cmdList, cache, localToWorld, renderComponent);
    });

  drawCtx.commandLists.push_back(cmdList.take_cmd_list());
}

} // namespace basalt::gfx
