#include "scene_view.h"

#include "types.h"
#include <runtime/scene/transform.h>

#include <utility>

namespace basalt::gfx {

using backend::RenderCommandList;

SceneView::SceneView(std::shared_ptr<Scene> scene, const Camera& camera)
  : mScene {std::move(scene)}, mCamera {camera} {
}

auto SceneView::draw(const Size2Du16 viewport) -> RenderCommandList {
  RenderCommandList commandList {
    mCamera.view_matrix(), mCamera.projection_matrix(viewport)
  , mScene->background_color()
  };

  commandList.set_ambient_light(mScene->ambient_light());
  commandList.set_directional_lights(mScene->directional_lights());

  const auto& ecs = mScene->ecs();

  ecs.view<const RenderComponent>().each(
    [&commandList, &ecs](
    const entt::entity entity, const RenderComponent& renderComponent
  ) {
      backend::RenderCommand command;

      if (ecs.has<Transform>(entity)) {
        const auto& transform = ecs.get<Transform>(
          entity);
        command.mWorld = math::Mat4f32::scaling(transform.mScale) *
          math::Mat4f32::rotation(transform.mRotation) *
          math::Mat4f32::translation(transform.mPosition);
      } else {
        command.mWorld = math::Mat4f32::identity();
      }

      command.mMesh = renderComponent.mMesh;
      command.model = renderComponent.model;
      command.mTexture = renderComponent.mTexture;
      command.mDiffuseColor = renderComponent.mDiffuseColor;
      command.mAmbientColor = renderComponent.mAmbientColor;
      command.texTransform = renderComponent.texTransform;
      command.texCoordinateSrc = renderComponent.tcs;
      command.mFlags = renderComponent.mRenderFlags;
      commandList.add(command);
    });

  return commandList;
}

auto SceneView::clear_color() const -> Color {
  return mScene->background_color();
}

} // namespace basalt::gfx
