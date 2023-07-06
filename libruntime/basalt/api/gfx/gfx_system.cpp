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

auto record_material(FilteringCommandList& cmdList, const MaterialData& data)
  -> void {
  cmdList.bind_pipeline(data.pipeline);
  cmdList.bind_texture(data.texture);
  cmdList.bind_sampler(data.sampler);

  cmdList.set_material(data.diffuse, data.ambient, Color {});
}

} // namespace

auto GfxSystem::on_update(const UpdateContext& ctx) -> void {
  Scene& scene {ctx.scene};
  EntityRegistry& entities {scene.entity_registry()};

  const View::DrawContext& drawCtx {
    *entities.ctx().get<const View::DrawContext*>()};

  FilteringCommandList cmdList {};
  cmdList.clear_attachments(
    Attachments {Attachment::RenderTarget, Attachment::DepthBuffer},
    scene.background(), 1.0f, 0);

  const Camera& camera {*entities.ctx().get<Camera*>()};
  cmdList.set_transform(TransformState::ViewToViewport,
                        camera.view_to_viewport(drawCtx.viewport));
  cmdList.set_transform(TransformState::WorldToView, camera.world_to_view());

  cmdList.set_ambient_light(scene.ambient_light());

  const auto& directionalLights {scene.directional_lights()};
  if (!directionalLights.empty()) {
    const vector<Light> lights {directionalLights.begin(),
                                directionalLights.end()};
    cmdList.set_lights(lights);
  }

  const auto& cache {drawCtx.cache};

  entities.view<const LocalToWorld, const ext::XModel>().each(
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

  entities.view<const LocalToWorld, const RenderComponent>().each(
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

  drawCtx.commandLists.push_back(cmdList.take_cmd_list());
}

} // namespace basalt::gfx
