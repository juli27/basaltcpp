#include "Matrices.h"

#include <array>

namespace scenes {
namespace d3d9tuts {


MatricesScene::MatricesScene(bs::gfx::backend::IRenderer* renderer)
  : m_renderer(renderer)
  , m_triangle{} {
  BS_ASSERT_ARG_NOT_NULL(renderer);

  bs::math::Vec3f32 cameraPos(0.0f, 3.0f, -5.0f);
  bs::math::Vec3f32 lookAt(0.0f, 0.0f, 0.0f);
  bs::math::Vec3f32 up(0.0f, 1.0f, 0.0f);
  m_view = bs::math::Mat4f32::Camera(cameraPos, lookAt, up);

  const bs::math::Vec2i32 windowSize = bs::platform::GetWindowDesc().size;
  const float aspectRatio =
    static_cast<float>(windowSize.GetX()) / windowSize.GetY();
  m_projection = bs::math::Mat4f32::PerspectiveProjection(
    bs::math::PI / 4.0f, aspectRatio, 1.0f, 100.0f
  );

  struct Vertex final {
    bs::f32 x, y, z;
    bs::u32 color;
  };

  std::array<Vertex, 3u> vertices{{
    {-1.0f, -1.0f, 0.0f, bs::Color(255, 0  , 0  ).ToARGB()},
    { 1.0f, -1.0f, 0.0f, bs::Color(0  , 0  , 255).ToARGB()},
    { 0.0f,  1.0f, 0.0f, bs::Color(255, 255, 255).ToARGB()}
  }};

  bs::gfx::backend::VertexLayout vertexLayout{
    {
      bs::gfx::backend::VertexElementUsage::POSITION,
      bs::gfx::backend::VertexElementType::F32_3
    },
    {
      bs::gfx::backend::VertexElementUsage::COLOR_DIFFUSE,
      bs::gfx::backend::VertexElementType::U32_1
    }
  };

  m_triangle.mesh = m_renderer->AddMesh(
    vertices.data(), vertices.size(), vertexLayout,
    bs::gfx::backend::PrimitiveType::TRIANGLE_LIST
  );
}


void MatricesScene::OnUpdate() {
  using namespace bs::gfx::backend;

  m_triangle.angle += static_cast<bs::f32>(bs::math::PI * 2.0 * bs::GetDeltaTime());
  if (m_triangle.angle >= bs::math::PI * 2.0f) {
    m_triangle.angle = 0.0f;
  }

  m_renderer->SetViewProj(m_view, m_projection);
  m_renderer->SetClearColor(bs::Color(0, 0, 255));

  RenderCommand command{};
  command.mesh = m_triangle.mesh;
  command.world = bs::math::Mat4f32::RotationY(m_triangle.angle);
  command.flags = RF_DISABLE_LIGHTING | RF_CULL_NONE;
  m_renderer->Submit(command);
}

} // namespace d3d9tuts
} // namespace scenes
