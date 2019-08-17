#include "Vertices.h"

#include <array>
#include <memory>
#include <tuple>

namespace d3d9_tuts {


Vertices::Vertices() : mScene(std::make_shared<bs::Scene>()) {
  mScene->SetBackgroundColor(bs::Color(0, 0, 255));

  struct Vertex final {
    bs::f32 x, y, z, rhw;
    bs::u32 color;
  };

  std::array<Vertex, 3u> vertices = {{
    {150.0f,  50.0f, 0.5f, 1.0f, bs::Color(255, 0  , 0).ToARGB()},
    {250.0f, 250.0f, 0.5f, 1.0f, bs::Color(0  , 255, 0).ToARGB()},
    { 50.0f, 250.0f, 0.5f, 1.0f, bs::Color(0  , 255, 255).ToARGB()}
  }};

  const bs::gfx::backend::VertexLayout vertexLayout{
    {
      bs::gfx::backend::VertexElementUsage::POSITION_TRANSFORMED,
      bs::gfx::backend::VertexElementType::F32_4
    },
    {
      bs::gfx::backend::VertexElementUsage::COLOR_DIFFUSE,
      bs::gfx::backend::VertexElementType::U32_1
    }
  };

  auto& entityRegistry = mScene->GetEntityRegistry();
  const auto triangleEntity = entityRegistry.create<bs::TransformComponent, bs::gfx::RenderComponent>();
  std::get<2>(triangleEntity).mMesh = bs::gfx::GetRenderer()->AddMesh(
    vertices.data(), static_cast<bs::i32>(vertices.size()), vertexLayout,
    bs::gfx::backend::PrimitiveType::TRIANGLE_LIST
  );
}


void Vertices::OnShow() {
  bs::SetCurrentScene(mScene);
}


void Vertices::OnHide() {}


void Vertices::OnUpdate() {}

} // namespace d3d9_tuts
