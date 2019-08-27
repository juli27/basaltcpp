#include "pch.h"

#include <basalt/gfx/Gfx.h>

#include <memory>

#include <basalt/Scene.h>
#include <basalt/gfx/Camera.h>
#include <basalt/gfx/RenderComponent.h>
#include <basalt/gfx/backend/IRenderer.h>
#include <basalt/gfx/backend/Types.h>

#include <basalt/Log.h>
#include <basalt/common/Asserts.h>

namespace basalt::gfx {

void render(backend::IRenderer* renderer, const std::shared_ptr<Scene>& scene) {
  BS_ASSERT(renderer, "gfx::render needs a Renderer");

  renderer->SetClearColor(scene->GetBackgroundColor());

  const auto& camera = scene->GetCamera();
  renderer->SetViewProj(camera.GetViewMatrix(), camera.GetProjectionMatrix());

  scene->GetEntityRegistry().view<const TransformComponent, const RenderComponent>().each(
    [renderer](const TransformComponent& transform, const RenderComponent& renderComponent) {
    backend::RenderCommand command;
    command.world = math::Mat4f32::Scaling(transform.mScale)
      * math::Mat4f32::Rotation(transform.mRotation)
      * math::Mat4f32::Translation(transform.mPosition);

    command.mesh = renderComponent.mMesh;
    command.texture = renderComponent.mTexture;
    command.diffuseColor = renderComponent.mDiffuseColor;
    command.ambientColor = renderComponent.mAmbientColor;
    command.flags = renderComponent.mRenderFlags;
    renderer->Submit(command);
  });

  renderer->Render();
}

} // namespace basalt::gfx
