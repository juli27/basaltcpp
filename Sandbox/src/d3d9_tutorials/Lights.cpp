#include "Lights.h"

#include <cmath> // sinf, cosf

#include <array>
#include <memory> // make_shared
#include <tuple> // get

#include <BasaltPrelude.h> // GetDeltaTime, SetCurrentScene, gfx

using std::array;

using bs::Color;
using bs::Scene;
using bs::TransformComponent;
using bs::math::PI;
using bs::math::Vec3f32;
using bs::gfx::Camera;
using bs::gfx::RenderComponent;
using bs::gfx::backend::LightSetup;
using bs::gfx::backend::PrimitiveType;
using bs::gfx::backend::RF_CULL_NONE;
using bs::gfx::backend::VertexElementType;
using bs::gfx::backend::VertexElementUsage;
using bs::gfx::backend::VertexLayout;

namespace d3d9_tuts {

Lights::Lights() : mScene(std::make_shared<Scene>()) {
  mScene->SetBackgroundColor(Color(0, 0, 255));

  const Vec3f32 cameraPos(0.0f, 3.0f, -5.0f);
  const Vec3f32 lookAt(0.0f, 0.0f, 0.0f);
  const Vec3f32 up(0.0f, 1.0f, 0.0f);
  mScene->SetCamera(Camera(cameraPos, lookAt, up));

  struct Vertex final {
    Vec3f32 mPos;
    Vec3f32 mNormal;
  };

  array<Vertex, 50u * 2> vertices;
  for(uSize i = 0u; i < 50u; i++) {
    const auto theta = ( 2.0f * PI * i ) / ( 50 - 1 );
    const auto sinTheta = std::sinf(theta);
    const auto cosTheta = std::cosf(theta);
    vertices[2 * i].mPos = {sinTheta, -1.0f, cosTheta};
    vertices[2 * i].mNormal = {sinTheta, 0.0f, cosTheta};
    vertices[2 * i + 1].mPos = {sinTheta, 1.0f, cosTheta};
    vertices[2 * i + 1].mNormal = {sinTheta, 0.0f, cosTheta};
  }

  const VertexLayout vertexLayout{
    { VertexElementUsage::POSITION, VertexElementType::F32_3 },
    { VertexElementUsage::NORMAL, VertexElementType::F32_3 }
  };

  auto& entityRegistry = mScene->GetEntityRegistry();
  auto entity = entityRegistry.create<TransformComponent, RenderComponent>();
  mCylinderEntity = std::get<0>(entity);

  auto& renderComponent = std::get<2>(entity);
  renderComponent.mMesh = bs::get_renderer()->AddMesh(
    vertices.data(), static_cast<i32>(vertices.size()), vertexLayout,
    PrimitiveType::TRIANGLE_STRIP
  );
  renderComponent.mDiffuseColor = Color(255, 255, 0);
  renderComponent.mAmbientColor = Color(255, 255, 0);
  renderComponent.mRenderFlags = RF_CULL_NONE;
}

void Lights::OnShow() {
  bs::SetCurrentScene(mScene);
}

void Lights::OnHide() {}

void Lights::OnUpdate() {
  const auto deltaTime = static_cast<f32>(bs::GetDeltaTime());
  const auto radOffetX = PI * 0.5f * deltaTime;
  auto& transform =
    mScene->GetEntityRegistry().get<TransformComponent>(mCylinderEntity);
  transform.Rotate(radOffetX, 0.0f, 0.0f);

  auto* renderer = bs::get_renderer();

  LightSetup lights;
  lights.SetGlobalAmbientColor(Color(32, 32, 32));

  mLightAngle += PI * 0.25f * deltaTime;
  // reset when rotated 360°
  while (mLightAngle >= PI * 2.0f) {
    mLightAngle -= PI * 2.0f;
  }

  const Vec3f32 lightDir(
    std::cosf(mLightAngle), 1.0f, std::sinf(mLightAngle)
  );
  lights.AddDirectionalLight(
    Vec3f32::Normalize(lightDir), Color(255, 255, 255)
  );

  renderer->SetLights(lights);
}

} // namespace d3d9_tuts
