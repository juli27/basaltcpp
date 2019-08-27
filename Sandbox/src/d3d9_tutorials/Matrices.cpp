#include "Matrices.h"

#include <array>
#include <memory> // make_shared
#include <tuple> // get

#include <BasaltPrelude.h> // GetDeltaTime, SetCurrentScene, gfx, ImGui

using std::array;

using bs::Color;
using bs::Scene;
using bs::TransformComponent;
using bs::math::Vec3f32;
using bs::gfx::Camera;
using bs::gfx::RenderComponent;
using bs::gfx::backend::PrimitiveType;
using bs::gfx::backend::RF_CULL_NONE;
using bs::gfx::backend::RF_DISABLE_LIGHTING;
using bs::gfx::backend::VertexElementType;
using bs::gfx::backend::VertexElementUsage;
using bs::gfx::backend::VertexLayout;

namespace d3d9_tuts {

Matrices::Matrices() : mScene(std::make_shared<Scene>()) {
  mScene->SetBackgroundColor(Color(0, 0, 0));

  const Vec3f32 cameraPos(0.0f, 3.0f, -5.0f);
  const Vec3f32 lookAt(0.0f, 0.0f, 0.0f);
  const Vec3f32 up(0.0f, 1.0f, 0.0f);
  mScene->SetCamera(Camera(cameraPos, lookAt, up));

  struct Vertex final {
    f32 x, y, z;
    u32 color;
  };

  array<Vertex, 3u> vertices{{
    {-1.0f, -1.0f, 0.0f, Color(255, 0  , 0  ).ToARGB()},
    { 1.0f, -1.0f, 0.0f, Color(0  , 0  , 255).ToARGB()},
    { 0.0f,  1.0f, 0.0f, Color(255, 255, 255).ToARGB()}
  }};

  const VertexLayout vertexLayout = {
    { VertexElementUsage::POSITION, VertexElementType::F32_3 },
    { VertexElementUsage::COLOR_DIFFUSE, VertexElementType::U32_1 }
  };

  auto& entityRegistry = mScene->GetEntityRegistry();
  auto entity = entityRegistry.create<TransformComponent, RenderComponent>();
  mTriangleEntity = std::get<0>(entity);
  auto& renderComponent = std::get<2>(entity);
  renderComponent.mMesh = bs::get_renderer()->AddMesh(
    vertices.data(), static_cast<i32>(vertices.size()), vertexLayout,
    PrimitiveType::TRIANGLE_LIST
  );
  renderComponent.mRenderFlags = RF_CULL_NONE | RF_DISABLE_LIGHTING;
}

void Matrices::OnShow() {
  bs::SetCurrentScene(mScene);
}

void Matrices::OnHide() {}

void Matrices::OnUpdate() {
  const auto deltaTime = static_cast<f32>(bs::GetDeltaTime());

  static auto speed = 2.0f;
  const auto radOffsetY = deltaTime * speed * bs::math::PI;
  auto& transform =
    mScene->GetEntityRegistry().get<TransformComponent>(mTriangleEntity);
  transform.Rotate(0.0f, radOffsetY, 0.0f);

  ImGui::Begin("D3D9 Tutorial 03: Matrices");
  ImGui::SliderFloat("Rotation Speed", &speed, -2.0f, 2.0f);
  ImGui::End();
}

} // namespace d3d9_tuts
