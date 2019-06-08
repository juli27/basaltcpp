#include "Vertices.h"

#include <array>

VerticesScene::VerticesScene(bs::gfx::backend::IRenderer* renderer) : m_renderer(renderer) {
  BS_ASSERT_ARG_NOT_NULL(renderer);

  struct Vertex final {
    float x, y, z, rhw;
    bs::u32 color;
  };

  std::array<Vertex, 3u> vertices{{
    {150.0f,  50.0f, 0.5f, 1.0f, bs::Color(255, 0  , 0).ToARGB()},
    {250.0f, 250.0f, 0.5f, 1.0f, bs::Color(0  , 255, 0).ToARGB()},
    { 50.0f, 250.0f, 0.5f, 1.0f, bs::Color(0  , 255, 255).ToARGB()}
  }};


  bs::gfx::backend::VertexLayout vertexLayout;
  vertexLayout.m_elements.push_back(
    {
      bs::gfx::backend::VertexElementUsage::POSITION_TRANSFORMED,
      bs::gfx::backend::VertexElementType::F32_4,
    }
  );
  vertexLayout.m_elements.push_back(
    {
      bs::gfx::backend::VertexElementUsage::COLOR_DIFFUSE,
      bs::gfx::backend::VertexElementType::U32_1,
    }
  );

  m_triangleMesh = m_renderer->AddMesh(
    {
      vertices.data(), vertices.size(), vertexLayout,
      bs::gfx::backend::PrimitiveType::TRIANGLE_LIST
    }
  );
}


void VerticesScene::OnUpdate() {
  m_renderer->Submit({m_triangleMesh});
}
