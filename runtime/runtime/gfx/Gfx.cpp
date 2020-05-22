#include "runtime/gfx/Gfx.h"

#include "runtime/gfx/Camera.h"
#include "runtime/gfx/types.h"

#include "runtime/gfx/backend/IRenderer.h"
#include "runtime/gfx/backend/render_command.h"

#include "runtime/scene/scene.h"
#include "runtime/scene/transform.h"

#include "runtime/shared/Asserts.h"

namespace basalt::gfx {

using backend::RenderCommandList;

void render(backend::IRenderer* renderer, const View& view) {
  BASALT_ASSERT(renderer);

  renderer->set_clear_color(view.scene->background_color());

  const auto& camera = view.camera;
  RenderCommandList commandList {
    camera.view_matrix(), camera.projection_matrix()
  };

  commandList.set_ambient_light(view.scene->ambient_light());
  commandList.set_directional_lights(view.scene->directional_lights());

  const auto& registry = view.scene->get_entity_registry();

  registry.view<const RenderComponent>().each(
    [&commandList, &registry](
    const entt::entity entity, const RenderComponent& renderComponent
  ) {
      backend::RenderCommand command;

      if (registry.has<Transform>(entity)) {
        const auto& transform = registry.get<Transform>(
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

  renderer->render(commandList);
}

} // namespace basalt::gfx
