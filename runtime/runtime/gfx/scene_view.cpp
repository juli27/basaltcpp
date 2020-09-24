#include "scene_view.h"

#include "types.h"
#include "backend/device.h"

#include <runtime/scene/transform.h>
#include <runtime/scene/scene.h>

#include <runtime/shared/size2d.h>

#include <entt/entity/registry.hpp>

#include <utility>

namespace basalt::gfx {

SceneView::SceneView(std::shared_ptr<Scene> scene, const Camera& camera)
  : mScene {std::move(scene)}, mCamera {camera} {
}

auto SceneView::draw(Device& device, const Size2Du16 viewport) -> CommandList {
  CommandList commandList {
    mCamera.view_matrix(), mCamera.projection_matrix(viewport)
  , mScene->background_color()
  };

  commandList.set_ambient_light(mScene->ambient_light());
  commandList.set_directional_lights(mScene->directional_lights());

  const auto& ecs = mScene->ecs();

  ecs.view<const Model>().each(
    [this, &device, &commandList, &ecs](
    const entt::entity entity, const Model& model) -> void {
      RenderCommandLegacy command {};
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
    [&commandList, &ecs](
    const entt::entity entity, const RenderComponent& renderComponent
  ) {
      RenderCommandLegacy command;

      if (ecs.has<Transform>(entity)) {
        const auto& transform = ecs.get<Transform>(
          entity);
        command.worldTransform = Mat4f32::scaling(transform.scale) *
          Mat4f32::rotation(transform.rotation) *
          Mat4f32::translation(transform.position);
      }

      command.mesh = renderComponent.mesh;
      command.texture = renderComponent.texture;
      command.diffuseColor = renderComponent.diffuseColor;
      command.ambientColor = renderComponent.ambientColor;
      command.texTransform = renderComponent.texTransform;
      command.texCoordinateSrc = renderComponent.tcs;
      command.flags = renderComponent.renderFlags;
      commandList.add(command);
    });

  return commandList;
}

auto SceneView::clear_color() const -> Color {
  return mScene->background_color();
}

} // namespace basalt::gfx
