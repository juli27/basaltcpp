#include "Lights.h"

#include <cmath>

#include <array>
#include <memory>
#include <tuple>

namespace d3d9_tuts {


Lights::Lights()
: mScene(std::make_shared<bs::Scene>())
, mLightAngle(0.0f) {
  mScene->SetBackgroundColor(bs::Color(0, 0, 255));

  const bs::math::Vec3f32 cameraPos(0.0f, 3.0f, -5.0f);
  const bs::math::Vec3f32 lookAt(0.0f, 0.0f, 0.0f);
  const bs::math::Vec3f32 up(0.0f, 1.0f, 0.0f);
  mScene->SetCamera(bs::gfx::Camera(cameraPos, lookAt, up));

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

  const bs::gfx::backend::VertexLayout vertexLayout{
    {
      bs::gfx::backend::VertexElementUsage::POSITION,
      bs::gfx::backend::VertexElementType::F32_3
    },
    {
      bs::gfx::backend::VertexElementUsage::NORMAL,
      bs::gfx::backend::VertexElementType::F32_3
    }
  };

  auto& entityRegistry = mScene->GetEntityRegistry();
  auto entity = entityRegistry.create<bs::TransformComponent, bs::gfx::RenderComponent>();
  mCylinderEntity = std::get<0>(entity);

  bs::gfx::RenderComponent& renderComponent = std::get<2>(entity);
  renderComponent.mMesh = bs::gfx::GetRenderer()->AddMesh(
    vertices.data(), static_cast<bs::i32>(vertices.size()), vertexLayout,
    bs::gfx::backend::PrimitiveType::TRIANGLE_STRIP
  );
  renderComponent.mDiffuseColor = bs::Color(255, 255, 0);
  renderComponent.mAmbientColor = bs::Color(255, 255, 0);
  renderComponent.mRenderFlags = bs::gfx::backend::RF_CULL_NONE;
}


void Lights::OnShow() {
  bs::SetCurrentScene(mScene);
}


void Lights::OnHide() {}


void Lights::OnUpdate() {
  const auto deltaTime = static_cast<bs::f32>(bs::GetDeltaTime());
  auto& transform =
    mScene->GetEntityRegistry().get<bs::TransformComponent>(mCylinderEntity);

  float rotationX = transform.mRotation.GetX();
  rotationX += bs::math::PI * 0.5f * deltaTime;
  // reset when rotated 360°
  while (rotationX >= bs::math::PI * 2.0f) {
    rotationX -= bs::math::PI * 2.0f;
  }

  transform.mRotation.SetX(rotationX);

  auto* renderer = bs::gfx::GetRenderer();

  bs::gfx::backend::LightSetup lights;
  lights.SetGlobalAmbientColor(bs::Color(32, 32, 32));

  mLightAngle += bs::math::PI * 0.25f * deltaTime;
  // reset when rotated 360°
  while (mLightAngle >= bs::math::PI * 2.0f) {
    mLightAngle -= bs::math::PI * 2.0f;
  }

  bs::math::Vec3f32 lightDir(
    std::cosf(mLightAngle), 1.0f, std::sinf(mLightAngle)
  );
  lights.AddDirectionalLight(
    bs::math::Vec3f32::Normalize(lightDir), bs::Color(255, 255, 255)
  );

  renderer->SetLights(lights);
}

} // namespace d3d9_tuts
