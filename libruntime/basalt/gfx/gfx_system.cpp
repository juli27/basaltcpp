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

template <class>
inline constexpr bool always_false_v = false;

auto record_camera(FilteringCommandList& cmdList, Scene& scene,
                   f32 const aspectRatio) -> void {
  auto& entities = scene.entity_registry();
  auto const cameraEntity = CameraEntity{
    scene.get_handle(entities.ctx().get<EntityId>(GfxSystem::sMainCamera))};

  cameraEntity.get_camera().aspectRatio = aspectRatio;

  cmdList.set_transform(TransformState::ViewToClip,
                        cameraEntity.view_to_clip());
  cmdList.set_transform(TransformState::WorldToView,
                        cameraEntity.world_to_view());
}

auto record_material(FilteringCommandList& cmdList, Context const& ctx,
                     MaterialHandle const id) -> void {
  auto const& data = ctx.get(id);

  cmdList.bind_pipeline(data.pipeline);
  cmdList.bind_texture(0, data.texture);
  cmdList.bind_sampler(0, data.sampler);

  cmdList.set_material(data.diffuse, data.ambient, data.emissive, data.specular,
                       data.specularPower);
  cmdList.set_fog_parameters(data.fogColor, data.fogStart, data.fogEnd,
                             data.fogDensity);
}

auto record_render_component(FilteringCommandList& cmdList, Context const& ctx,
                             LocalToWorld const& localToWorld,
                             RenderComponent const& renderComponent) {
  record_material(cmdList, ctx, renderComponent.material);

  cmdList.set_transform(TransformState::Texture0, renderComponent.texTransform);
  cmdList.set_transform(TransformState::LocalToWorld, localToWorld.matrix);

  auto const& meshData = ctx.get(renderComponent.mesh);
  cmdList.bind_vertex_buffer(meshData.vertexBuffer);

  if (meshData.indexBuffer) {
    cmdList.bind_index_buffer(meshData.indexBuffer);
    cmdList.draw_indexed(0, 0, meshData.vertexCount, 0, meshData.indexCount);
  } else {
    cmdList.draw(meshData.startVertex, meshData.vertexCount);
  }
}

} // namespace

auto GfxSystem::on_update(UpdateContext const& ctx) -> void {
  auto& scene = ctx.scene;
  auto& entities = scene.entity_registry();
  auto const& ecsCtx{entities.ctx()};

  auto const& drawCtx = ecsCtx.get<View::DrawContext const>();
  auto const& gfxCtx = ecsCtx.get<Context const>();
  auto const& env = ecsCtx.get<Environment const>();

  auto cmdList = FilteringCommandList{};
  cmdList.clear_attachments(
    Attachments{Attachment::RenderTarget, Attachment::DepthBuffer},
    env.background(), 1.0f);

  auto const aspectRatio = drawCtx.viewport.aspect_ratio();
  record_camera(cmdList, scene, aspectRatio);

  cmdList.set_ambient_light(env.ambient_light());

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

  cmdList.set_lights(lights);

  entities.view<LocalToWorld const, ext::XModelHandle const>().each(
    [&](LocalToWorld const& localToWorld, ext::XModelHandle const& model) {
      cmdList.set_transform(TransformState::LocalToWorld, localToWorld.matrix);

      auto const& modelData = gfxCtx.get(model);

      auto const numMaterials = static_cast<u32>(modelData.materials.size());

      for (auto i = uSize{0}; i < numMaterials; ++i) {
        record_material(cmdList, gfxCtx, modelData.materials[i]);

        ext::XMeshCommandEncoder::draw_x_mesh(cmdList.cmd_list(),
                                              modelData.meshes[i]);
      }
    });

  entities.view<LocalToWorld const, RenderComponent const>().each(
    [&](LocalToWorld const& localToWorld,
        RenderComponent const& renderComponent) {
      record_render_component(cmdList, gfxCtx, localToWorld, renderComponent);
    });

  drawCtx.commandLists.push_back(cmdList.take_cmd_list());
}

} // namespace basalt::gfx
