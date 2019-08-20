#include "Textures.h"

#include <cmath> // sinf, cosf

#include <array>
#include <memory>
#include <tuple>

#include <Basalt.h>

namespace d3d9_tuts {


Textures::Textures() : mScene(std::make_shared<bs::Scene>()) {
  mScene->SetBackgroundColor(bs::Color(0, 0, 255));
  mScene->SetCamera(bs::gfx::Camera(
    {0.0f, 3.0f, -5.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}
  ));

  struct Vertex final {
    bs::math::Vec3f32 pos;
    bs::u32 color = bs::Color().ToARGB();
    bs::math::Vec2f32 uv;
  };

  std::array<Vertex, 50u * 2> vertices;

  for(bs::uSize i = 0; i < 50; i++) {
    const auto theta = ( 2.0f * bs::math::PI * i ) / ( 50 - 1 );
    const auto sinTheta = std::sinf(theta);
    const auto cosTheta = std::cosf(theta);

    vertices[2 * i].pos = {sinTheta, -1.0f, cosTheta};
    vertices[2 * i].color = bs::Color(255, 255, 255).ToARGB();
    vertices[2 * i].uv = {i / (50.0f - 1), 1.0f};

    vertices[2 * i + 1].pos = {sinTheta, 1.0f, cosTheta};
    vertices[2 * i + 1].color = bs::Color(128, 128, 128).ToARGB();
    vertices[2 * i + 1].uv = {i / (50.0f - 1), 0.0f};
  }

  const bs::gfx::backend::VertexLayout vertexLayout{
    {
      bs::gfx::backend::VertexElementUsage::POSITION,
      bs::gfx::backend::VertexElementType::F32_3
    },
    {
      bs::gfx::backend::VertexElementUsage::COLOR_DIFFUSE,
      bs::gfx::backend::VertexElementType::U32_1
    },
    {
      bs::gfx::backend::VertexElementUsage::TEXTURE_COORDS,
      bs::gfx::backend::VertexElementType::F32_2
    }
  };

  auto& entityRegistry = mScene->GetEntityRegistry();
  auto entity = entityRegistry.create<bs::TransformComponent, bs::gfx::RenderComponent>();
  mCylinderEntity = std::get<0>(entity);

  bs::gfx::RenderComponent& renderComponent = std::get<2>(entity);
  auto* renderer = bs::gfx::GetRenderer();
  renderComponent.mMesh = renderer->AddMesh(
    vertices.data(), static_cast<bs::i32>(vertices.size()), vertexLayout,
    bs::gfx::backend::PrimitiveType::TRIANGLE_STRIP
  );
  renderComponent.mTexture = renderer->AddTexture("data/banana.bmp");
  renderComponent.mRenderFlags =
    bs::gfx::backend::RF_CULL_NONE | bs::gfx::backend::RF_DISABLE_LIGHTING;
}


void Textures::OnShow() {
  bs::SetCurrentScene(mScene);
}


void Textures::OnHide() {}


void Textures::OnUpdate() {
  const auto deltaTime = static_cast<bs::f32>(bs::GetDeltaTime());
  const auto radOffetX = bs::math::PI * 0.5f * deltaTime;
  auto& transform =
    mScene->GetEntityRegistry().get<bs::TransformComponent>(mCylinderEntity);
  transform.Rotate(radOffetX, 0.0f, 0.0f);

  mScene->DisplayEntityGui(mCylinderEntity);
}

} // namespace d3d9_tuts
