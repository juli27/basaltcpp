#include "scene_view.h"

#include "command_list_recorder.h"
#include "types.h"

#include "backend/command_list.h"
#include "backend/commands.h"
#include "backend/device.h"

#include "runtime/scene/transform.h"
#include "runtime/scene/scene.h"

#include "runtime/shared/size2d.h"
#include "runtime/shared/utils.h"

#include <entt/entity/registry.hpp>

#include <utility>

using std::optional;

namespace basalt::gfx {

SceneView::SceneView(std::shared_ptr<Scene> scene, const Camera& camera)
  : mScene {std::move(scene)}, mCamera {camera} {
}

auto SceneView::draw(Device& device, const Size2Du16 viewport) -> CommandList {
  CommandListRecorder cmdListRecorder {};

  cmdListRecorder.set_transform(TransformType::View, mCamera.view_matrix());
  cmdListRecorder.set_transform(TransformType::Projection,
                                mCamera.projection_matrix(viewport));

  cmdListRecorder.set_ambient_light(mScene->ambient_light());

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
        cmdListRecorder.set_transform(TransformType::World, worldTransform);
      }

      if (mModelCache.find(model.model) == mModelCache.end()) {
        mModelCache[model.model] = device.load_model(model.model);
      }

      CommandLegacy command {};
      command.model = mModelCache[model.model];

      cmdListRecorder.add(command);
    });

  ecs.view<const RenderComponent>().each(
    [&](const entt::entity entity, const RenderComponent& renderComponent) {
      if (const auto* transform = ecs.try_get<Transform>(entity)) {
        const auto worldTransform = Mat4f32::scaling(transform->scale) *
                                    Mat4f32::rotation(transform->rotation) *
                                    Mat4f32::translation(transform->position);
        cmdListRecorder.set_transform(TransformType::World, worldTransform);
      }

      const auto lightingEnabled =
        !(renderComponent.renderFlags & RenderFlagDisableLighting);
      cmdListRecorder.set_render_state(RenderState::Lighting, lightingEnabled);

      auto getCullMode = [](const u8 renderFlags) {
        return renderFlags & RenderFlagCullNone ? CullModeNone : CullModeCcw;
      };
      cmdListRecorder.set_render_state(
        RenderState::CullMode, getCullMode(renderComponent.renderFlags));

      CommandLegacy command {};
      command.mesh = renderComponent.mesh;
      command.texture = renderComponent.texture;
      command.diffuseColor = renderComponent.diffuseColor;
      command.ambientColor = renderComponent.ambientColor;
      command.texTransform = renderComponent.texTransform;
      command.texCoordinateSrc = renderComponent.tcs;
      cmdListRecorder.add(command);
    });

  return cmdListRecorder.complete_command_list();
}

auto SceneView::clear_color() const -> optional<Color> {
  return mScene->background_color();
}

} // namespace basalt::gfx
