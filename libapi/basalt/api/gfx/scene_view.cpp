#include <basalt/api/gfx/scene_view.h>

#include <basalt/api/gfx/filtering_command_list.h>
#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/types.h>

#include <basalt/api/gfx/backend/render_state.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/scene/transform.h>
#include <basalt/api/scene/scene.h>

#include <basalt/api/math/mat4.h>
#include <basalt/api/math/rectangle.h>

#include <basalt/api/shared/size2d.h>

#include <entt/entity/registry.hpp>

#include <variant>
#include <utility>

namespace basalt::gfx {

namespace {

void record_material(FilteringCommandList& cmdList, const MaterialData& data) {
  cmdList.set_render_state(RenderState::cull_mode(
    std::get<CullMode>(data.renderStates[RenderStateType::CullMode])));
  cmdList.set_render_state(RenderState::lighting(
    std::get<bool>(data.renderStates[RenderStateType::Lighting])));
  cmdList.set_render_state(RenderState::fill_mode(
    std::get<FillMode>(data.renderStates[RenderStateType::FillMode])));

  cmdList.bind_texture(data.texture);
  cmdList.bind_sampler(data.sampler);

  cmdList.set_texture_stage_state(
    0, TextureStageState::CoordinateSource,
    data.textureStageStates[TextureStageState::CoordinateSource]);
  cmdList.set_texture_stage_state(
    0, TextureStageState::TextureTransformFlags,
    data.textureStageStates[TextureStageState::TextureTransformFlags]);

  cmdList.set_material(data.diffuse, data.ambient, Color {});
}

} // namespace

SceneView::SceneView(ScenePtr scene, const Camera& camera)
  : mScene {std::move(scene)}, mCamera {camera} {
}

auto SceneView::camera() const noexcept -> const Camera& {
  return mCamera;
}

auto SceneView::draw(ResourceCache& cache, const Size2Du16 viewport,
                     const RectangleU16&)
  -> std::tuple<CommandList, RectangleU16> {
  FilteringCommandList cmdList {};
  cmdList.clear_attachments(
    Attachments {Attachment::Color, Attachment::ZBuffer}, mScene->background(),
    1.0f, 0);

  cmdList.set_transform(TransformState::Projection,
                        mCamera.projection_matrix(viewport));
  cmdList.set_transform(TransformState::View, mCamera.view_matrix());

  cmdList.set_render_state(RenderState::ambient(mScene->ambient_light()));

  const auto& directionalLights {mScene->directional_lights()};
  if (!directionalLights.empty()) {
    cmdList.set_directional_lights(directionalLights);
  }

  const auto& ecs {mScene->ecs()};

  ecs.view<const Transform, const ext::XModel>().each(
    [&](const Transform& transform, const ext::XModel& model) {
      const auto objToWorldMatrix {Mat4f32::scaling(transform.scale) *
                                   Mat4f32::rotation(transform.rotation) *
                                   Mat4f32::translation(transform.position)};
      cmdList.set_transform(TransformState::World, objToWorldMatrix);

      cmdList.ext_draw_x_model(model);
    });

  ecs.view<const Transform, const RenderComponent>().each(
    [&](const Transform& transform, const RenderComponent& renderComponent) {
      const MaterialData& materialData {cache.get(renderComponent.material)};
      record_material(cmdList, materialData);

      cmdList.set_transform(TransformState::Texture,
                            renderComponent.texTransform);

      const auto objToWorldMatrix {Mat4f32::scaling(transform.scale) *
                                   Mat4f32::rotation(transform.rotation) *
                                   Mat4f32::translation(transform.position)};
      cmdList.set_transform(TransformState::World, objToWorldMatrix);

      const auto& meshData = cache.get(renderComponent.mesh);
      cmdList.bind_vertex_buffer(meshData.vertexBuffer, 0ull);
      cmdList.draw(meshData.startVertex, meshData.primitiveType,
                   meshData.primitiveCount);
    });

  return {cmdList.take_cmd_list(), viewport.to_rectangle()};
}

} // namespace basalt::gfx
