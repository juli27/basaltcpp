#include "scene_view.h"

#include <basalt/api/gfx/command_list_recorder.h>
#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/types.h>

#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/scene/transform.h>
#include <basalt/api/scene/scene.h>

#include <basalt/api/math/mat4.h>
#include <basalt/api/math/rectangle.h>

#include <basalt/api/shared/size2d.h>

#include <basalt/api/base/utils.h>

#include <entt/entity/registry.hpp>

#include <utility>

namespace basalt::gfx {

namespace {

void record_material(CommandListRecorder& cmdList,
                     const MaterialData& material) {
  cmdList.set_render_state(RenderState::CullMode,
                           material.renderStates[RenderState::CullMode]);
  cmdList.set_render_state(RenderState::Lighting,
                           material.renderStates[RenderState::Lighting]);

  cmdList.set_texture_stage_state(
    0, TextureStageState::CoordinateSource,
    material.textureStageStates[TextureStageState::CoordinateSource]);

  cmdList.set_texture_stage_state(
    0, TextureStageState::TextureTransformFlags,
    material.textureStageStates[TextureStageState::TextureTransformFlags]);

  cmdList.set_material(material.diffuse, material.ambient, Color {});
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
  CommandListRecorder cmdList;
  cmdList.clear(mScene->background());

  cmdList.set_transform(TransformState::Projection,
                        mCamera.projection_matrix(viewport));
  cmdList.set_transform(TransformState::View, mCamera.view_matrix());

  cmdList.set_render_state(RenderState::Ambient,
                           enum_cast(mScene->ambient_light().to_argb()));

  const auto& directionalLights = mScene->directional_lights();
  if (!directionalLights.empty()) {
    cmdList.set_directional_lights(directionalLights);
  }

  const auto& ecs = mScene->ecs();

  ecs.view<const ext::XModel>().each(
    [&, this](const entt::entity entity, const ext::XModel& model) {
      if (const auto* transform = ecs.try_get<Transform>(entity)) {
        const auto worldTransform = Mat4f32::scaling(transform->scale) *
                                    Mat4f32::rotation(transform->rotation) *
                                    Mat4f32::translation(transform->position);
        cmdList.set_transform(TransformState::World, worldTransform);
      }

      cmdList.ext_draw_x_model(model);
    });

  ecs.view<const RenderComponent>().each(
    [&](const entt::entity entity, const RenderComponent& renderComponent) {
      if (renderComponent.material) {
        const auto& materialData = cache.get(renderComponent.material);

        record_material(cmdList, materialData);
      }

      if (const auto* transform = ecs.try_get<Transform>(entity)) {
        const auto worldTransform = Mat4f32::scaling(transform->scale) *
                                    Mat4f32::rotation(transform->rotation) *
                                    Mat4f32::translation(transform->position);
        cmdList.set_transform(TransformState::World, worldTransform);
      }

      if (renderComponent.texTransform != Mat4f32::identity()) {
        cmdList.set_transform(TransformState::Texture,
                              renderComponent.texTransform);
      }

      if (renderComponent.texture) {
        cmdList.set_texture(renderComponent.texture);
      }

      const auto& meshData = cache.get(renderComponent.mesh);
      cmdList.draw(meshData.vertexBuffer, meshData.primitiveType,
                   meshData.startVertex, meshData.primitiveCount);
    });

  return {cmdList.take_cmd_list(), viewport.to_rectangle()};
}

} // namespace basalt::gfx
