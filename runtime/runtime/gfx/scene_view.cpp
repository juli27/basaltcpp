#include "scene_view.h"

#include "types.h"
#include "backend/command_list.h"
#include "backend/commands.h"
#include "backend/device.h"

#include "runtime/scene/transform.h"
#include "runtime/scene/scene.h"

#include "runtime/shared/size2d.h"

#include <entt/entity/registry.hpp>

#include <utility>

using std::optional;

namespace basalt::gfx {

SceneView::SceneView(std::shared_ptr<Scene> scene, const Camera& camera)
  : mScene {std::move(scene)}, mCamera {camera} {
}

auto SceneView::draw(Device& device, const Size2Du16 viewport) -> CommandList {
  CommandList commandList {};

  commandList.set_transform(TransformType::View, mCamera.view_matrix());
  commandList.set_transform(TransformType::Projection,
                            mCamera.projection_matrix(viewport));

  commandList.set_ambient_light(mScene->ambient_light());

  const auto& directionalLights = mScene->directional_lights();
  if (!directionalLights.empty()) {
    commandList.set_directional_lights(directionalLights);
  }

  const auto& ecs = mScene->ecs();

  ecs.view<const Model>().each(
    [&, this](const entt::entity entity, const Model& model) {
      CommandLegacy command {};
      if (ecs.has<Transform>(entity)) {
        const auto& transform = ecs.get<Transform>(entity);
        command.worldTransform = Mat4f32::scaling(transform.scale) *
                                 Mat4f32::rotation(transform.rotation) *
                                 Mat4f32::translation(transform.position);
      }

      if (mModelCache.find(model.model) == mModelCache.end()) {
        mModelCache[model.model] = device.load_model(model.model);
      }

      command.model = mModelCache[model.model];

      commandList.add(command);
    });

  ecs.view<const RenderComponent>().each(
    [&](const entt::entity entity, const RenderComponent& renderComponent) {
      CommandLegacy command;

      if (const auto* transform = ecs.try_get<Transform>(entity)) {
        command.worldTransform = Mat4f32::scaling(transform->scale) *
                                 Mat4f32::rotation(transform->rotation) *
                                 Mat4f32::translation(transform->position);
      }

      if (renderComponent.renderFlags & RenderFlagDisableLighting) {
        commandList.set_render_state(RenderState::Lighting, false);
      }
      if (renderComponent.renderFlags & RenderFlagCullNone) {
        commandList.set_render_state(RenderState::CullMode, CullModeNone);
      }

      command.mesh = renderComponent.mesh;
      command.texture = renderComponent.texture;
      command.diffuseColor = renderComponent.diffuseColor;
      command.ambientColor = renderComponent.ambientColor;
      command.texTransform = renderComponent.texTransform;
      command.texCoordinateSrc = renderComponent.tcs;
      commandList.add(command);

      if (renderComponent.renderFlags & RenderFlagCullNone) {
        commandList.set_render_state(RenderState::CullMode, CullModeCcw);
      }
      if (renderComponent.renderFlags & RenderFlagDisableLighting) {
        commandList.set_render_state(RenderState::Lighting, true);
      }
    });

  return commandList;
}

auto SceneView::clear_color() const -> optional<Color> {
  return mScene->background_color();
}

} // namespace basalt::gfx
