#include "scene_view.h"

#include "command_list_recorder.h"
#include "resource_cache.h"
#include "types.h"

#include "backend/types.h"

#include "api/scene/transform.h"
#include "api/scene/scene.h"

#include "api/math/mat4.h"

#include "api/shared/size2d.h"

#include "api/base/utils.h"

#include <entt/entity/registry.hpp>

#include <utility>

using std::optional;

namespace basalt::gfx {

namespace {

void record_material(CommandListRecorder& cmdListRecorder,
                     const MaterialData& material) {
  cmdListRecorder.set_render_state(
    RenderState::CullMode, material.renderStates[RenderState::CullMode]);
  cmdListRecorder.set_render_state(
    RenderState::Lighting, material.renderStates[RenderState::Lighting]);

  cmdListRecorder.set_texture_stage_state(
    0, TextureStageState::CoordinateSource,
    material.textureStageStates[TextureStageState::CoordinateSource]);

  cmdListRecorder.set_texture_stage_state(
    0, TextureStageState::TextureTransformFlags,
    material.textureStageStates[TextureStageState::TextureTransformFlags]);

  cmdListRecorder.set_material(material.diffuse, material.ambient, Color {});
}

} // namespace

SceneView::SceneView(ScenePtr scene, const Camera& camera)
  : mScene {std::move(scene)}, mCamera {camera} {
}

auto SceneView::camera() const noexcept -> const Camera& {
  return mCamera;
}

auto SceneView::draw(ResourceCache& cache, const Size2Du16 viewport)
  -> CommandList {
  CommandListRecorder cmdListRecorder;

  cmdListRecorder.set_transform(TransformState::Projection,
                                mCamera.projection_matrix(viewport));
  cmdListRecorder.set_transform(TransformState::View, mCamera.view_matrix());

  cmdListRecorder.set_render_state(
    RenderState::Ambient, enum_cast(mScene->ambient_light().to_argb()));

  const auto& directionalLights = mScene->directional_lights();
  if (!directionalLights.empty()) {
    cmdListRecorder.set_directional_lights(directionalLights);
  }

  const auto& ecs = mScene->ecs();

  ecs.view<const ext::XModel>().each(
    [&, this](const entt::entity entity, const ext::XModel& model) {
      if (const auto* transform = ecs.try_get<Transform>(entity)) {
        const auto worldTransform = Mat4f32::scaling(transform->scale) *
                                    Mat4f32::rotation(transform->rotation) *
                                    Mat4f32::translation(transform->position);
        cmdListRecorder.set_transform(TransformState::World, worldTransform);
      }

      cmdListRecorder.ext_draw_x_model(model);
    });

  ecs.view<const RenderComponent>().each(
    [&](const entt::entity entity, const RenderComponent& renderComponent) {
      if (renderComponent.material) {
        const auto& materialData = cache.get(renderComponent.material);

        record_material(cmdListRecorder, materialData);
      }

      if (const auto* transform = ecs.try_get<Transform>(entity)) {
        const auto worldTransform = Mat4f32::scaling(transform->scale) *
                                    Mat4f32::rotation(transform->rotation) *
                                    Mat4f32::translation(transform->position);
        cmdListRecorder.set_transform(TransformState::World, worldTransform);
      }

      if (renderComponent.texTransform != Mat4f32::identity()) {
        cmdListRecorder.set_transform(TransformState::Texture,
                                      renderComponent.texTransform);
      }

      if (renderComponent.texture) {
        cmdListRecorder.set_texture(renderComponent.texture);
      }

      cmdListRecorder.draw(renderComponent.mesh);
    });

  return cmdListRecorder.complete_command_list();
}

auto SceneView::clear_color() const -> optional<Color> {
  return mScene->background();
}

} // namespace basalt::gfx
