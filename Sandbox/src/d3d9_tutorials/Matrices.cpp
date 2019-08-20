#include "Matrices.h"

#include <array>
#include <memory>
#include <tuple>

namespace d3d9_tuts {


Matrices::Matrices() : mScene(std::make_shared<bs::Scene>()) {
  mScene->SetBackgroundColor(bs::Color(0, 0, 0));

  const bs::math::Vec3f32 cameraPos(0.0f, 3.0f, -5.0f);
  const bs::math::Vec3f32 lookAt(0.0f, 0.0f, 0.0f);
  const bs::math::Vec3f32 up(0.0f, 1.0f, 0.0f);
  mScene->SetCamera(bs::gfx::Camera(cameraPos, lookAt, up));

  struct Vertex final {
    bs::f32 x, y, z;
    bs::u32 color;
  };

  std::array<Vertex, 3u> vertices{{
    {-1.0f, -1.0f, 0.0f, bs::Color(255, 0  , 0  ).ToARGB()},
    { 1.0f, -1.0f, 0.0f, bs::Color(0  , 0  , 255).ToARGB()},
    { 0.0f,  1.0f, 0.0f, bs::Color(255, 255, 255).ToARGB()}
  }};

  const bs::gfx::backend::VertexLayout vertexLayout{
    {
      bs::gfx::backend::VertexElementUsage::POSITION,
      bs::gfx::backend::VertexElementType::F32_3
    },
    {
      bs::gfx::backend::VertexElementUsage::COLOR_DIFFUSE,
      bs::gfx::backend::VertexElementType::U32_1
    }
  };

  auto& entityRegistry = mScene->GetEntityRegistry();
  auto entity = entityRegistry.create<bs::TransformComponent, bs::gfx::RenderComponent>();
  mTriangleEntity = std::get<0>(entity);
  bs::gfx::RenderComponent& renderComponent = std::get<2>(entity);
  renderComponent.mMesh = bs::gfx::GetRenderer()->AddMesh(
    vertices.data(), static_cast<bs::i32>(vertices.size()), vertexLayout,
    bs::gfx::backend::PrimitiveType::TRIANGLE_LIST
  );
  renderComponent.mRenderFlags =
    bs::gfx::backend::RF_CULL_NONE | bs::gfx::backend::RF_DISABLE_LIGHTING;
}


void Matrices::OnShow() {
  bs::SetCurrentScene(mScene);
}


void Matrices::OnHide() {}


void Matrices::OnUpdate() {
  const auto deltaTime = static_cast<bs::f32>(bs::GetDeltaTime());

  static auto speed = 2.0f;
  const auto radOffsetY = deltaTime * speed * bs::math::PI;
  auto& transform =
    mScene->GetEntityRegistry().get<bs::TransformComponent>(mTriangleEntity);
  transform.Rotate(0.0f, radOffsetY, 0.0f);

  ImGui::Begin("D3D9 Tutorial 03: Matrices");
  ImGui::SliderFloat("Rotation Speed", &speed, -2.0f, 2.0f);
  ImGui::End();
}

} // namespace d3d9_tuts
