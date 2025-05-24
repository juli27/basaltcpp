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

struct VbRenderMesh {
  VertexBufferSlice vbSlice;
};

struct IndexedVbRenderMesh {
  VertexBufferSlice vbSlice;
  IndexBufferSlice ibSlice;
};

using RenderMesh =
  std::variant<VbRenderMesh, IndexedVbRenderMesh, ext::XMeshHandle>;

struct DrawCall {
  MaterialData materialData;
  LocalToWorld objectToScene;
  RenderMesh renderMesh;
};

} // namespace

auto GfxSystem::on_update(UpdateContext const& ctx) -> void {
  auto& scene = ctx.scene;
  auto const& entities = scene.entity_registry();
  auto const& ecsCtx = entities.ctx();
  auto const& gfxCtx = ecsCtx.get<Context const>();

  auto needsDepth = false;
  auto needsLights = false;

  auto const drawCalls = [&] {
    auto drawCalls = vector<DrawCall>{};

    entities.view<LocalToWorld const, ext::XModel const>().each(
      [&](LocalToWorld const& localToWorld, ext::XModel const& model) {
        drawCalls.push_back(
          DrawCall{gfxCtx.get(model.material), localToWorld, model.mesh});

        auto const& drawCall = drawCalls.back();
        auto const& materialFeatures = drawCall.materialData.features;

        needsDepth |= materialFeatures.has(MaterialFeature::DepthBuffer);
        needsLights |= materialFeatures.has(MaterialFeature::Lighting);
      });

    entities.view<LocalToWorld const, Model const>().each(
      [&](LocalToWorld const& localToWorld, Model const& model) {
        auto const& meshData = gfxCtx.get(model.mesh);

        auto const renderMesh = [&]() -> RenderMesh {
          if (meshData.indexBuffer) {
            return IndexedVbRenderMesh{
              VertexBufferSlice{meshData.vertexBuffer, meshData.startVertex,
                                meshData.vertexCount},
              IndexBufferSlice{meshData.indexBuffer, 0, meshData.indexCount}};
          }

          return VbRenderMesh{VertexBufferSlice{
            meshData.vertexBuffer, meshData.startVertex, meshData.vertexCount}};
        }();

        drawCalls.push_back(
          DrawCall{gfxCtx.get(model.material), localToWorld, renderMesh});

        auto const& drawCall = drawCalls.back();
        auto const& materialFeatures = drawCall.materialData.features;

        needsDepth |= materialFeatures.has(MaterialFeature::DepthBuffer);
        needsLights |= materialFeatures.has(MaterialFeature::Lighting);
      });

    return drawCalls;
  }();

  auto const& env = ecsCtx.get<Environment const>();

  auto const& drawCtx = ecsCtx.get<View::DrawContext const>();
  auto cmdList = FilteringCommandList{};

  auto clearAttachments = Attachments{Attachment::RenderTarget};
  if (needsDepth) {
    clearAttachments.set(Attachment::DepthBuffer);
  }

  auto const& clearColorValue = env.background();
  auto const clearDepthValue = 1.0f;
  cmdList.clear_attachments(clearAttachments, clearColorValue, clearDepthValue);

  if (drawCalls.empty()) {
    drawCtx.commandLists.push_back(cmdList.take_cmd_list());

    return;
  }

  auto const cameraEntityId = ecsCtx.get<EntityId>(GfxSystem::sMainCamera);
  auto const cameraEntity = CameraEntity{scene.get_handle(cameraEntityId)};
  cameraEntity.get_camera().aspectRatio = drawCtx.viewport.aspect_ratio();
  auto const viewToClip = cameraEntity.view_to_clip();
  cmdList.set_transform(TransformState::ViewToClip, viewToClip);
  cmdList.set_transform(TransformState::WorldToView,
                        cameraEntity.world_to_view());

  if (needsLights) {
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
  }

  for (auto const& drawCall : drawCalls) {
    auto const& materialData = drawCall.materialData;
    cmdList.bind_pipeline(materialData.pipeline);

    auto const& materialFeatures = materialData.features;
    if (materialFeatures.has(MaterialFeature::Texturing)) {
      cmdList.bind_texture(0, materialData.texture);
      cmdList.bind_sampler(0, materialData.sampler);
    }

    if (materialFeatures.has(MaterialFeature::UniformColors)) {
      cmdList.set_material(materialData.diffuse, materialData.ambient,
                           materialData.emissive, materialData.specular,
                           materialData.specularPower);
    }

    if (materialFeatures.has(MaterialFeature::Fog)) {
      cmdList.set_fog_parameters(materialData.fogColor, materialData.fogStart,
                                 materialData.fogEnd, materialData.fogDensity);
    }

    if (materialFeatures.has(MaterialFeature::TexCoordTransform)) {
      // TODO: remove hack
      // HACK: this is temporary
      cmdList.set_transform(TransformState::Texture0,
                            viewToClip * materialData.texTransform);
    }

    cmdList.set_transform(TransformState::LocalToWorld,
                          drawCall.objectToScene.matrix);

    std::visit(Overloaded{
                 [&](VbRenderMesh const& m) {
                   cmdList.bind_vertex_buffer(m.vbSlice.buffer);
                   cmdList.draw(m.vbSlice.start, m.vbSlice.count);
                 },
                 [&](IndexedVbRenderMesh const& m) {
                   cmdList.bind_vertex_buffer(m.vbSlice.buffer);
                   cmdList.bind_index_buffer(m.ibSlice.buffer);
                   cmdList.draw_indexed(0, 0, m.vbSlice.count, m.ibSlice.start,
                                        m.ibSlice.count);
                 },
                 [&](ext::XMeshHandle const& m) {
                   ext::XMeshCommandEncoder::draw_x_mesh(cmdList.cmd_list(), m);
                 },
               },
               drawCall.renderMesh);
  }

  drawCtx.commandLists.push_back(cmdList.take_cmd_list());
}

} // namespace basalt::gfx
