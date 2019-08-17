#include "pch.h"

#include <basalt/gfx/Gfx.h>

#include <memory>

#include <basalt/Scene.h>
#include <basalt/gfx/Camera.h>
#include <basalt/gfx/RenderComponent.h>
#include <basalt/gfx/backend/IRenderer.h>
#include <basalt/gfx/backend/Types.h>

#include <basalt/Log.h>

namespace basalt::gfx {
namespace {


backend::IRenderer* sRenderer;

} // namespace


void Init() {
  sRenderer = backend::IRenderer::Create(backend::RendererType::Default);
  BS_INFO("gfx backend: {}", sRenderer->GetName());
}


void Shutdown() {
  if (sRenderer) {
    delete sRenderer;
    sRenderer = nullptr;
  }
}


void Render(const std::shared_ptr<Scene>& scene) {
  sRenderer->SetClearColor(scene->GetBackgroundColor());

  const auto& camera = scene->GetCamera();
  sRenderer->SetViewProj(camera.GetViewMatrix(), camera.GetProjectionMatrix());

  scene->GetEntityRegistry().view<const TransformComponent, const RenderComponent>().each(
    [](const TransformComponent& transform, const RenderComponent& renderComponent) {
    backend::RenderCommand command;
    command.world = math::Mat4f32::Scaling(transform.mScale)
      * math::Mat4f32::Rotation(transform.mRotation)
      * math::Mat4f32::Translation(transform.mPosition);

    command.mesh = renderComponent.mMesh;
    command.texture = renderComponent.mTexture;
    command.diffuseColor = renderComponent.mDiffuseColor;
    command.ambientColor = renderComponent.mAmbientColor;
    command.flags = renderComponent.mRenderFlags;
    sRenderer->Submit(command);
  });

  sRenderer->Render();
}


void Present() {
  sRenderer->Present();
}


auto GetRenderer() -> backend::IRenderer* {
  return sRenderer;
}

} // namespace basalt::gfx
