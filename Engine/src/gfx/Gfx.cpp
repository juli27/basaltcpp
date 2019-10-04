#include <basalt/gfx/Gfx.h>

#include <basalt/Scene.h>
#include <basalt/gfx/Camera.h>
#include <basalt/gfx/RenderComponent.h>
#include <basalt/gfx/backend/IRenderer.h>
#include <basalt/gfx/backend/Types.h>

#include <basalt/Log.h>
#include <basalt/common/Asserts.h>

#include <memory>

namespace basalt::gfx {

void render(backend::IRenderer* renderer, const std::shared_ptr<Scene>& scene) {
  BS_ASSERT(renderer, "gfx::render needs a Renderer");

  renderer->set_clear_color(scene->get_background_color());

  const auto& camera = scene->get_camera();
  renderer->set_view_proj(camera.get_view_matrix(), camera.get_projection_matrix());

  scene->get_entity_registry().view<const TransformComponent, const RenderComponent>().each(
    [renderer](const TransformComponent& transform, const RenderComponent& renderComponent) {
    backend::RenderCommand command;
    command.mWorld = math::Mat4f32::scaling(transform.mScale)
      * math::Mat4f32::rotation(transform.mRotation)
      * math::Mat4f32::translation(transform.mPosition);

    command.mMesh = renderComponent.mMesh;
    command.mTexture = renderComponent.mTexture;
    command.mDiffuseColor = renderComponent.mDiffuseColor;
    command.mAmbientColor = renderComponent.mAmbientColor;
    command.mFlags = renderComponent.mRenderFlags;
    renderer->submit(command);
  });

  renderer->render();
}

} // namespace basalt::gfx
