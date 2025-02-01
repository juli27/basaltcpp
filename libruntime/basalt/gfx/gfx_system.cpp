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

// TODO: buffer slice types [X]BufferSlice
//       with ownership of the buffer?

struct VertexBufferSlice {
  VertexBufferHandle buffer;
  u32 start;
  u32 count;
};

struct IndexBufferSlice {
  IndexBufferHandle buffer;
  u32 start;
  u32 count;
};

struct RenderMeshDrawCall {
  LocalToWorld objectToScene;
  MaterialHandle material;
  VertexBufferSlice vbSlice;
};

struct IndexedRenderMeshDrawCall {
  LocalToWorld objectToScene;
  MaterialHandle material;
  VertexBufferSlice vbSlice;
  IndexBufferSlice ibSlice;
};

struct XMeshDrawCall {
  LocalToWorld objectToScene;
  MaterialHandle material;
  ext::XMeshHandle mesh;
};

using DrawCall =
  std::variant<RenderMeshDrawCall, IndexedRenderMeshDrawCall, XMeshDrawCall>;

} // namespace

auto GfxSystem::on_update(UpdateContext const& ctx) -> void {
  auto& scene = ctx.scene;
  auto& entities = scene.entity_registry();
  auto const& ecsCtx{entities.ctx()};
  auto const& gfxCtx = ecsCtx.get<Context const>();

  auto const drawCalls = [&] {
    auto drawCalls = vector<DrawCall>();

    entities.view<LocalToWorld const, ext::XModel const>().each(
      [&](LocalToWorld const& localToWorld, ext::XModel const& model) {
        drawCalls.push_back(
          XMeshDrawCall{localToWorld, model.material, model.mesh});
      });

    entities.view<LocalToWorld const, Model const>().each(
      [&](LocalToWorld const& localToWorld,
          Model const& renderComponent) {
        auto const& meshData = gfxCtx.get(renderComponent.mesh);

        if (meshData.indexBuffer) {
          drawCalls.push_back(IndexedRenderMeshDrawCall{
            localToWorld, renderComponent.material,
            VertexBufferSlice{meshData.vertexBuffer, meshData.startVertex,
                              meshData.vertexCount},
            IndexBufferSlice{meshData.indexBuffer, 0, meshData.indexCount}});
        } else {
          drawCalls.push_back(RenderMeshDrawCall{
            localToWorld, renderComponent.material,
            VertexBufferSlice{meshData.vertexBuffer, meshData.startVertex,
                              meshData.vertexCount}});
        }
      });

    return drawCalls;
  }();

  auto const& drawCtx = ecsCtx.get<View::DrawContext const>();

  auto cmdList = FilteringCommandList{};

  auto const& env = ecsCtx.get<Environment const>();
  cmdList.clear_attachments(
    Attachments{Attachment::RenderTarget, Attachment::DepthBuffer},
    env.background(), 1.0f);

  auto const cameraEntityId = ecsCtx.get<EntityId>(GfxSystem::sMainCamera);
  auto const cameraEntity = CameraEntity{scene.get_handle(cameraEntityId)};
  cameraEntity.get_camera().aspectRatio = drawCtx.viewport.aspect_ratio();
  cmdList.set_transform(TransformState::ViewToClip,
                        cameraEntity.view_to_clip());
  cmdList.set_transform(TransformState::WorldToView,
                        cameraEntity.world_to_view());

  auto const lights = [&] {
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

  auto const recordMaterial = [&](MaterialData const& data) {
    cmdList.bind_pipeline(data.pipeline);
    cmdList.bind_texture(0, data.texture);
    cmdList.bind_sampler(0, data.sampler);

    cmdList.set_material(data.diffuse, data.ambient, data.emissive,
                         data.specular, data.specularPower);
    cmdList.set_fog_parameters(data.fogColor, data.fogStart, data.fogEnd,
                               data.fogDensity);
  };

  for (auto const& drawCall : drawCalls) {
    std::visit(Overloaded{
                 [&](RenderMeshDrawCall const& d) {
                   auto const& materialData = gfxCtx.get(d.material);
                   recordMaterial(materialData);

                   cmdList.set_transform(TransformState::LocalToWorld,
                                         d.objectToScene.matrix);

                   cmdList.bind_vertex_buffer(d.vbSlice.buffer);
                   cmdList.draw(d.vbSlice.start, d.vbSlice.count);
                 },
                 [&](IndexedRenderMeshDrawCall const& d) {
                   auto const& materialData = gfxCtx.get(d.material);
                   recordMaterial(materialData);

                   cmdList.set_transform(TransformState::LocalToWorld,
                                         d.objectToScene.matrix);

                   cmdList.bind_vertex_buffer(d.vbSlice.buffer);
                   cmdList.bind_index_buffer(d.ibSlice.buffer);
                   cmdList.draw_indexed(0, 0, d.vbSlice.count, d.ibSlice.start,
                                        d.ibSlice.count);
                 },
                 [&](XMeshDrawCall const& d) {
                   cmdList.set_transform(TransformState::LocalToWorld,
                                         d.objectToScene.matrix);

                   auto const& materialData = gfxCtx.get(d.material);
                   recordMaterial(materialData);

                   ext::XMeshCommandEncoder::draw_x_mesh(cmdList.cmd_list(),
                                                         d.mesh);
                 },
               },
               drawCall);
  }

  drawCtx.commandLists.push_back(cmdList.take_cmd_list());
}

} // namespace basalt::gfx
