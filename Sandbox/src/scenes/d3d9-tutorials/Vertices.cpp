#include "Vertices.h"

#include <array>

namespace scenes {
namespace d3d9tuts {


VerticesScene::VerticesScene(bs::gfx::backend::IRenderer* renderer)
  : m_renderer(renderer) {
  BS_ASSERT_ARG_NOT_NULL(renderer);

  struct Vertex final {
    bs::f32 x, y, z, rhw;
    bs::u32 color;
  };

  std::array<Vertex, 3u> vertices{{
    {150.0f,  50.0f, 0.5f, 1.0f, bs::Color(255, 0  , 0).ToARGB()},
    {250.0f, 250.0f, 0.5f, 1.0f, bs::Color(0  , 255, 0).ToARGB()},
    { 50.0f, 250.0f, 0.5f, 1.0f, bs::Color(0  , 255, 255).ToARGB()}
  }};

  bs::gfx::backend::VertexLayout vertexLayout{
    {
      bs::gfx::backend::VertexElementUsage::POSITION_TRANSFORMED,
      bs::gfx::backend::VertexElementType::F32_4
    },
    {
      bs::gfx::backend::VertexElementUsage::COLOR_DIFFUSE,
      bs::gfx::backend::VertexElementType::U32_1
    }
  };
  m_triangleMesh = m_renderer->AddMesh(
    vertices.data(), static_cast<bs::i32>(vertices.size()), vertexLayout,
    bs::gfx::backend::PrimitiveType::TRIANGLE_LIST
  );
}


void VerticesScene::OnUpdate() {
  m_renderer->SetClearColor(bs::Color(0, 0, 255));

  m_renderer->Submit({m_triangleMesh});

  ImGui::ShowDemoWindow();
}

} // namespace d3d9tuts
} // namespace scenes
