#include "Lights.h"

#include <cmath>

#include <array>

namespace scenes {
namespace d3d9tuts {


LightsScene::LightsScene(bs::gfx::backend::IRenderer* renderer)
  : m_renderer(renderer)
  , m_cylinder{}
  , m_lightAngle(0.0f) {
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
    bs::math::Vec3f32 pos;
    bs::math::Vec3f32 normal;
  };

  std::array<Vertex, 50u * 2> vertices;

  for(bs::i32 i = 0; i < 50; i++) {
    bs::f32 theta = ( 2.0f * bs::math::PI * i ) / ( 50 - 1 );
    bs::f32 sinTheta = std::sinf(theta);
    bs::f32 cosTheta = std::cosf(theta);
    vertices[2 * i].pos = {sinTheta, -1.0f, cosTheta};
    vertices[2 * i].normal = {sinTheta, 0.0f, cosTheta};
    vertices[2 * i + 1].pos = {sinTheta, 1.0f, cosTheta};
    vertices[2 * i + 1].normal = {sinTheta, 0.0f, cosTheta};
  }

  bs::gfx::backend::VertexLayout vertexLayout{
    {
      bs::gfx::backend::VertexElementUsage::POSITION,
      bs::gfx::backend::VertexElementType::F32_3
    },
    {
      bs::gfx::backend::VertexElementUsage::NORMAL,
      bs::gfx::backend::VertexElementType::F32_3
    }
  };

  m_cylinder.mesh = m_renderer->AddMesh(
    vertices.data(), static_cast<bs::i32>(vertices.size()), vertexLayout,
    bs::gfx::backend::PrimitiveType::TRIANGLE_STRIP
  );
}


void LightsScene::OnUpdate() {
  using namespace bs::gfx::backend;
  bs::f32 deltaTime = static_cast<bs::f32>(bs::GetDeltaTime());

  m_cylinder.angle += bs::math::PI * 0.5f * deltaTime;
  // reset when rotated 360°
  while (m_cylinder.angle >= bs::math::PI * 2.0f) {
    m_cylinder.angle -= bs::math::PI * 2.0f;
  }

  m_renderer->SetViewProj(m_view, m_projection);
  m_renderer->SetClearColor(bs::Color(0, 0, 255));

  bs::gfx::backend::LightSetup lights;
  lights.SetGlobalAmbientColor(bs::Color(32, 32, 32));

  m_lightAngle += bs::math::PI * 0.25f * deltaTime;
  // reset when rotated 360°
  while (m_lightAngle >= bs::math::PI * 2.0f) {
    m_lightAngle -= bs::math::PI * 2.0f;
  }

  bs::math::Vec3f32 lightDir(
    std::cosf(m_lightAngle), 1.0f, std::sinf(m_lightAngle)
  );
  lights.AddDirectionalLight(
    bs::math::Vec3f32::Normalize(lightDir), bs::Color(255, 255, 255)
  );

  m_renderer->SetLights(lights);

  RenderCommand command{};
  command.mesh = m_cylinder.mesh;
  command.diffuseColor = bs::Color(255, 255, 0);
  command.ambientColor = bs::Color(255, 255, 0);
  command.world = bs::math::Mat4f32::RotationX(m_cylinder.angle);
  command.flags = RF_CULL_NONE;
  m_renderer->Submit(command);
}

} // namespace d3d9tuts
} // namespace scenes
