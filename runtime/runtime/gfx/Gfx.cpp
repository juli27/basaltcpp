#include "runtime/gfx/Gfx.h"

#include "runtime/Scene.h"

#include "runtime/gfx/Camera.h"
#include "runtime/gfx/RenderComponent.h"

#include "runtime/gfx/backend/IRenderer.h"
#include "runtime/gfx/backend/render_command.h"

#include "runtime/shared/Asserts.h"

namespace basalt::gfx {

using backend::RenderCommandList;

void render(backend::IRenderer* renderer, Scene* const scene) {
  BASALT_ASSERT(renderer);

  renderer->set_clear_color(scene->background_color());

  const auto& camera = scene->camera();
  RenderCommandList commandList {
    camera.view_matrix(), camera.projection_matrix()
  };

  commandList.set_ambient_light(scene->ambient_light());

  const auto& registry = scene->get_entity_registry();

  registry.view<const RenderComponent>().each(
    [&commandList, &registry](
    const entt::entity entity, const RenderComponent& renderComponent
  ) {
      backend::RenderCommand command;

      if (registry.has<TransformComponent>(entity)) {
        const auto& transform = registry.get<TransformComponent>(
          entity);
        command.mWorld = math::Mat4f32::scaling(transform.mScale) *
          math::Mat4f32::rotation(transform.mRotation) *
          math::Mat4f32::translation(transform.mPosition);
      } else {
        command.mWorld = math::Mat4f32::identity();
      }

      command.mMesh = renderComponent.mMesh;
      command.mTexture = renderComponent.mTexture;
      command.mDiffuseColor = renderComponent.mDiffuseColor;
      command.mAmbientColor = renderComponent.mAmbientColor;
      command.mFlags = renderComponent.mRenderFlags;
      commandList.add(command);
    });

  renderer->render(commandList);
}

} // namespace basalt::gfx
