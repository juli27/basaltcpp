#include "Sandbox.h"

#include <cmath>

#include <array>

SandboxApp::SandboxApp(bs::Config& config)
  : m_verticesScene(nullptr) {
  config.mainWindow.title = "Basalt Sandbox";
  config.mainWindow.mode = bs::WindowMode::WINDOWED;
}

void SandboxApp::OnInit(bs::gfx::backend::IRenderer* renderer) {
  m_verticesScene = new VerticesScene(renderer);

  /*bs::math::Vec3f32 cameraPos(0.0f, 3.0f, -5.0f);
  bs::math::Vec3f32 lookAt(0.0f, 0.0f, 0.0f);
  bs::math::Vec3f32 up(0.0f, 1.0f, 0.0f);
  bs::math::Mat4f32 view = bs::math::Mat4f32::Camera(cameraPos, lookAt, up);

  const bs::math::Vec2i32 windowSize = bs::platform::GetWindowDesc().size;
  const float aspectRatio =
    static_cast<float>(windowSize.GetX()) / windowSize.GetY();
  bs::math::Mat4f32 proj = bs::math::Mat4f32::PerspectiveProjection(
    bs::math::PI / 4.0f, aspectRatio, 1.0f, 100.0f
  );

  m_renderer->SetViewProj(view, proj);

  struct Vertex {
    bs::math::Vec3f32 pos;
    bs::math::Vec3f32 normal;
    bs::math::Vec2f32 uv;
  };

  std::array<Vertex, 2 * 50u> meshData;

  for (int i = 0; i < 50; ++i) {
    const float theta = (2 * bs::math::PI * i) / (50 - 1);
    const float sinTheta = std::sinf(theta);
    const float cosTheta = std::cosf(theta);
    meshData[2 * i + 0].pos = { sinTheta, -1.0f, cosTheta };
    meshData[2 * i + 0].normal = { sinTheta, 0.0f, cosTheta };
    meshData[2 * i + 0].uv = { static_cast<float>(i) / (50 - 1), 1.0f };
    meshData[2 * i + 1].pos = { sinTheta, 1.0f, cosTheta };
    meshData[2 * i + 1].normal = { sinTheta, 0.0f, cosTheta };
    meshData[2 * i + 1].uv = { static_cast<float>(i) / (50 - 1), 0.0f };
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
  vertexLayout.m_elements.push_back(
    {
      bs::gfx::backend::VertexElementUsage::TEXTURE_COORDS,
      bs::gfx::backend::VertexElementType::F32_2
    }
  );
  m_triangleCommand.mesh = m_renderer->AddMesh(
    {
      meshData.data(), meshData.size(), vertexLayout,
      bs::gfx::backend::PrimitiveType::TRIANGLE_STRIP
    }
  );

  m_triangleCommand.diffuseColor = bs::Color::FromARGB(0x00ffff00);
  m_triangleCommand.ambientColor = bs::Color(255, 255, 0);

  m_triangleCommand.texture = m_renderer->AddTexture("data/banana.bmp");*/
}

void SandboxApp::OnShutdown() {
  if (m_verticesScene) {
    delete m_verticesScene;
    m_verticesScene = nullptr;
  }
}

void SandboxApp::OnUpdate() {
  /*static float angle = 0.0f;
  static float angle2 = 0.0f;

  if (!bs::input::IsKeyPressed(bs::Key::SPACE)) {
    angle += static_cast<float>(elapsedTimeInSeconds * 0.5f * bs::math::PI);
    m_triangleCommand.world = bs::math::Mat4f32::RotationX(angle);
  }

  angle2 += static_cast<float>(elapsedTimeInSeconds * 0.5f * bs::math::PI);
  bs::gfx::backend::LightSetup lights;
  lights.SetGlobalAmbientColor(bs::Color(32, 32, 32));
  bs::math::Vec3f32 lightDir(std::cosf(angle2), 1.0f, std::sinf(angle2));

  lights.AddDirectionalLight(
    bs::math::Vec3f32::Normalize(lightDir), bs::Color(255, 255, 255)
  );
  if (!bs::input::IsKeyPressed(bs::Key::L)) {
    lights.AddDirectionalLight(
      bs::math::Vec3f32::Normalize(-lightDir), bs::Color(0, 255, 0)
    );
  }

  m_renderer->SetLights(lights);

  m_renderer->Submit(m_triangleCommand);*/

  //ImGui::ShowDemoWindow();

  m_verticesScene->OnUpdate();

  if (bs::input::IsKeyPressed(bs::Key::ESCAPE)) {
    // TODO: use the Engine class for lifecycle management?
    // (but it is internal right now and would expose non useable functions)
    bs::platform::RequestQuit();
  }
}

bs::IApplication* bs::IApplication::Create(Config& config) {
  return new SandboxApp(config);
}
