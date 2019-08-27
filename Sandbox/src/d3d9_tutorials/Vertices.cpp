#include "Vertices.h"

#include <array>
#include <memory> // make_shared
#include <tuple> // get

#include <BasaltPrelude.h> // SetCurrentScene, gfx

using std::array;

using bs::Color;
using bs::Scene;
using bs::TransformComponent;
using bs::gfx::RenderComponent;
using bs::gfx::backend::PrimitiveType;
using bs::gfx::backend::VertexElementType;
using bs::gfx::backend::VertexElementUsage;
using bs::gfx::backend::VertexLayout;

namespace d3d9_tuts {

Vertices::Vertices() : mScene(std::make_shared<Scene>()) {
  mScene->SetBackgroundColor(Color(0, 0, 255));

  struct Vertex final {
    f32 x, y, z, rhw;
    u32 color;
  };

  array<Vertex, 3u> vertices = {{
    {150.0f,  50.0f, 0.5f, 1.0f, Color(255, 0  , 0).ToARGB()},
    {250.0f, 250.0f, 0.5f, 1.0f, Color(0  , 255, 0).ToARGB()},
    { 50.0f, 250.0f, 0.5f, 1.0f, Color(0  , 255, 255).ToARGB()}
  }};

  const VertexLayout vertexLayout{
    { VertexElementUsage::POSITION_TRANSFORMED, VertexElementType::F32_4 },
    { VertexElementUsage::COLOR_DIFFUSE, VertexElementType::U32_1 }
  };

  auto& entityRegistry = mScene->GetEntityRegistry();
  const auto triangleEntity = entityRegistry.create<TransformComponent, RenderComponent>();
  std::get<2>(triangleEntity).mMesh = bs::get_renderer()->AddMesh(
    vertices.data(), static_cast<i32>(vertices.size()), vertexLayout,
    PrimitiveType::TRIANGLE_LIST
  );
}

void Vertices::OnShow() {
  bs::SetCurrentScene(mScene);
}


void Vertices::OnHide() {}


void Vertices::OnUpdate() {}

} // namespace d3d9_tuts
