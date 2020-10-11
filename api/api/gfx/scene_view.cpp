#include "scene_view.h"

#include "command_list_recorder.h"
#include "resource_cache.h"
#include "types.h"

#include "backend/commands.h"
#include "backend/types.h"

#include "api/scene/transform.h"
#include "api/scene/scene.h"

#include "api/math/mat4.h"

#include "api/shared/size2d.h"
#include "api/shared/utils.h"

#include <entt/entity/registry.hpp>

#include <utility>

using std::optional;

namespace basalt::gfx {

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

  ecs.view<const Model>().each(
    [&, this](const entt::entity entity, const Model& model) {
      if (const auto* transform = ecs.try_get<Transform>(entity)) {
        const auto worldTransform = Mat4f32::scaling(transform->scale) *
                                    Mat4f32::rotation(transform->rotation) *
                                    Mat4f32::translation(transform->position);
        cmdListRecorder.set_transform(TransformState::World, worldTransform);
      }

      cmdListRecorder.ext_draw_x_model(cache.get(model.handle));
    });

  ecs.view<const RenderComponent>().each(
    [&](const entt::entity entity, const RenderComponent& renderComponent) {
      if (const auto* transform = ecs.try_get<Transform>(entity)) {
        const auto worldTransform = Mat4f32::scaling(transform->scale) *
                                    Mat4f32::rotation(transform->rotation) *
                                    Mat4f32::translation(transform->position);
        cmdListRecorder.set_transform(TransformState::World, worldTransform);
      }

      const auto lightingEnabled =
        !(renderComponent.renderFlags & RenderFlagDisableLighting);
      cmdListRecorder.set_render_state(RenderState::Lighting, lightingEnabled);

      auto getCullMode = [](const u8 renderFlags) {
        return renderFlags & RenderFlagCullNone ? CullModeNone : CullModeCcw;
      };
      cmdListRecorder.set_render_state(
        RenderState::CullMode, getCullMode(renderComponent.renderFlags));

      if (renderComponent.texTransform != Mat4f32::identity()) {
        cmdListRecorder.set_transform(TransformState::Texture,
                                      renderComponent.texTransform);
      }

      cmdListRecorder.set_texture_stage_state(
        0, TextureStageState::CoordinateSource, enum_cast(renderComponent.tcs));

      CommandLegacy command {};
      command.mesh = renderComponent.mesh;
      command.texture = cache.get(renderComponent.texture);
      command.diffuseColor = renderComponent.diffuseColor;
      command.ambientColor = renderComponent.ambientColor;
      command.texCoordinateSrc = renderComponent.tcs;
      cmdListRecorder.add(command);
    });

  return cmdListRecorder.complete_command_list();
}

auto SceneView::clear_color() const -> optional<Color> {
  return mScene->background();
}

} // namespace basalt::gfx
