#include "Sandbox.h"

#include <cmath>

#include <array>

SandboxApp::SandboxApp(bs::Config& config)
  : m_renderer(nullptr)
  , m_triangleCommand{} {
  config.mainWindow.title = "Basalt Sandbox";
  config.mainWindow.mode = bs::WindowMode::WINDOWED;
}

void SandboxApp::OnInit(bs::gfx::backend::IRenderer* renderer) {
  m_renderer = renderer;

  struct Vertex {
    bs::math::Vec3f32 pos;
    bs::math::Vec3f32 normal;
  };

  std::array<Vertex, 2 * 50u> meshData;

  for (int i = 0; i < 50; ++i) {
    const float theta = (2 * bs::math::PI * i) / (50 - 1);
    const float sinTheta = std::sinf(theta);
    const float cosTheta = std::cosf(theta);
    meshData[2 * i + 0].pos = { sinTheta, -1.0f, cosTheta };
    meshData[2 * i + 0].normal = { sinTheta, 0.0f, cosTheta };
    meshData[2 * i + 1].pos = { sinTheta, 1.0f, cosTheta };
    meshData[2 * i + 1].normal = { sinTheta, 0.0f, cosTheta };
  }
  
  bs::gfx::backend::VertexLayout vertexLayout;
  vertexLayout.m_elements.push_back(
    {
      bs::gfx::backend::VertexElementUsage::POSITION,
      bs::gfx::backend::VertexElementType::F32_3
    }
  );
  vertexLayout.m_elements.push_back(
    {
      bs::gfx::backend::VertexElementUsage::NORMAL,
      bs::gfx::backend::VertexElementType::F32_3
    }
  );
  m_triangleCommand.mesh = m_renderer->AddMesh(
    {
      vertexLayout, meshData.size(),
      bs::gfx::backend::PrimitiveType::TRIANGLE_STRIP, meshData.data()
    }
  );

  bs::math::Vec3f32 cameraPos(0.0f, 3.0f, -5.0f);
  bs::math::Vec3f32 lookAt(0.0f, 0.0f, 0.0f);
  bs::math::Vec3f32 up(0.0f, 1.0f, 0.0f);
  m_triangleCommand.view = bs::math::Mat4f32::Camera(cameraPos, lookAt, up);

  const bs::math::Vec2i32 windowSize = bs::platform::GetWindowDesc().size;
  const float aspectRatio =
    static_cast<float>(windowSize.GetX()) / windowSize.GetY();
  m_triangleCommand.proj = bs::math::Mat4f32::PerspectiveProjection(
    bs::math::PI / 4.0f, aspectRatio, 1.0f, 100.0f
  );
}

void SandboxApp::OnShutdown() {}

void SandboxApp::OnUpdate(double elapsedTimeInSeconds) {
  static float angle = 0.0f;

  if (!bs::input::IsKeyPressed(bs::Key::SPACE)) {
    angle += static_cast<float>(elapsedTimeInSeconds * 0.5f * bs::math::PI);
    m_triangleCommand.world = bs::math::Mat4f32::RotationX(angle);
  }

  m_renderer->Submit(m_triangleCommand);

  //ImGui::ShowDemoWindow();

  if (bs::input::IsKeyPressed(bs::Key::ESCAPE)) {
    // TODO: use the Engine class for lifecycle management?
    // (but it is internal right now and would expose non useable functions)
    bs::platform::RequestQuit();
  }
}

bs::IApplication* bs::IApplication::Create(Config& config) {
  return new SandboxApp(config);
}
