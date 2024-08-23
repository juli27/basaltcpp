#include <basalt/api/gfx/gfx_system.h>

#include "filtering_command_list.h"

#include <basalt/api/view.h> // for DrawContext ...

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/environment.h>
#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <basalt/api/scene/ecs.h>
#include <basalt/api/scene/scene.h>
#include <basalt/api/scene/transform.h>
#include <basalt/api/scene/types.h>

#include <basalt/api/base/functional.h>

#include <gsl/span>

#include <variant>
#include <vector>

using std::vector;

using gsl::span;

namespace basalt::gfx {

namespace {

auto record_camera(FilteringCommandList& cmdList,
                   CameraEntity const& cameraEntity) -> void {
  cmdList.set_transform(TransformState::ViewToClip,
                        cameraEntity.view_to_clip());
  cmdList.set_transform(TransformState::WorldToView,
                        cameraEntity.world_to_view());
}

auto record_material(FilteringCommandList& cmdList, MaterialData const& data)
  -> void {
  cmdList.bind_pipeline(data.pipeline);
  cmdList.bind_texture(0, data.texture);
  cmdList.bind_sampler(0, data.sampler);

  cmdList.set_material(data.diffuse, data.ambient, data.emissive, data.specular,
                       data.specularPower);
  cmdList.set_fog_parameters(data.fogColor, data.fogStart, data.fogEnd,
                             data.fogDensity);
}

} // namespace

auto GfxSystem::on_update(UpdateContext const& ctx) -> void {
  auto& scene = ctx.scene;
  auto& entities = scene.entity_registry();
  auto const& ecsCtx{entities.ctx()};

  auto const& drawCtx = ecsCtx.get<View::DrawContext const>();
  auto const& gfxCtx = ecsCtx.get<Context const>();
  auto const& env = ecsCtx.get<Environment const>();
  auto const cameraEntityId = ecsCtx.get<EntityId>(GfxSystem::sMainCamera);

  auto cmdList = FilteringCommandList{};
  cmdList.clear_attachments(
    Attachments{Attachment::RenderTarget, Attachment::DepthBuffer},
    env.background(), 1.0f);

  auto const cameraEntity = CameraEntity{scene.get_handle(cameraEntityId)};
  cameraEntity.get_camera().aspectRatio = drawCtx.viewport.aspect_ratio();
  record_camera(cmdList, cameraEntity);

  auto lights = [&] {
    auto lights = vector<LightData>{};
    auto const directionalLights = env.directional_lights();
    if (!directionalLights.empty()) {
      lights.insert(lights.end(), directionalLights.begin(),
                    directionalLights.end());
    }

    // TODO: this should use LocalToWorld
    entities.view<Transform const, Light const>().each(
      [&](Transform const& transform, Light const& light) {
        std::visit(Overloaded{
                     [&](PointLight const& l) {
                       lights.emplace_back(PointLightData{
                         l.diffuse, l.specular, l.ambient, transform.position,
                         l.range, l.attenuation0, l.attenuation1,
                         l.attenuation2});
                     },
                     [&](SpotLight const& l) {
                       lights.emplace_back(SpotLightData{
                         l.diffuse, l.specular, l.ambient, transform.position,
                         l.direction, l.range, l.attenuation0, l.attenuation1,
                         l.attenuation2, l.falloff, l.phi, l.theta});
                     },
                   },
                   light);
      });

    return lights;
  }();

  cmdList.set_ambient_light(env.ambient_light());
  cmdList.set_lights(lights);

  entities.view<LocalToWorld const, ext::XModelHandle const>().each(
    [&](LocalToWorld const& localToWorld, ext::XModelHandle const& model) {
      cmdList.set_transform(TransformState::LocalToWorld, localToWorld.matrix);

      auto const& modelData = gfxCtx.get(model);

      auto const numMaterials = modelData.materials.size();
      for (auto i = uSize{0}; i < numMaterials; ++i) {
        auto const& materialData = gfxCtx.get(modelData.materials[i]);
        record_material(cmdList, materialData);

        ext::XMeshCommandEncoder::draw_x_mesh(cmdList.cmd_list(),
                                              modelData.meshes[i]);
      }
    });

  entities.view<LocalToWorld const, RenderComponent const>().each(
    [&](LocalToWorld const& localToWorld,
        RenderComponent const& renderComponent) {
      auto const& materialData = gfxCtx.get(renderComponent.material);
      record_material(cmdList, materialData);

      cmdList.set_transform(TransformState::LocalToWorld, localToWorld.matrix);

      auto const& meshData = gfxCtx.get(renderComponent.mesh);
      cmdList.bind_vertex_buffer(meshData.vertexBuffer);

      if (meshData.indexBuffer) {
        cmdList.bind_index_buffer(meshData.indexBuffer);
        cmdList.draw_indexed(0, 0, meshData.vertexCount, 0,
                             meshData.indexCount);
      } else {
        cmdList.draw(meshData.startVertex, meshData.vertexCount);
      }
    });

  drawCtx.commandLists.push_back(cmdList.take_cmd_list());
}

} // namespace basalt::gfx
