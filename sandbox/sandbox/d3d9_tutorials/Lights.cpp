#include "Lights.h"

#include <runtime/Prelude.h> // GetDeltaTime, SetCurrentScene, gfx

#include <array>
#include <memory> // make_shared
#include <tuple> // get

#include <cmath> // sinf, cosf

using std::array;

using basalt::Color;
using basalt::Scene;
using basalt::TransformComponent;
using basalt::math::PI;
using basalt::math::Vec3f32;
using basalt::gfx::Camera;
using basalt::gfx::RenderComponent;
using basalt::gfx::backend::LightSetup;
using basalt::gfx::backend::PrimitiveType;
using basalt::gfx::backend::RenderFlagCullNone;
using basalt::gfx::backend::VertexElementType;
using basalt::gfx::backend::VertexElementUsage;
using basalt::gfx::backend::VertexLayout;

namespace d3d9_tuts {

Lights::Lights() : mScene(std::make_shared<Scene>()) {
  mScene->set_background_color(Color(0, 0, 255));

  const Vec3f32 cameraPos(0.0f, 3.0f, -5.0f);
  const Vec3f32 lookAt(0.0f, 0.0f, 0.0f);
  const Vec3f32 up(0.0f, 1.0f, 0.0f);
  mScene->set_camera(Camera(cameraPos, lookAt, up));

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
    { VertexElementUsage::Position, VertexElementType::F32_3 },
    { VertexElementUsage::Normal, VertexElementType::F32_3 }
  };

  auto& entityRegistry = mScene->get_entity_registry();
  auto entity = entityRegistry.create<TransformComponent, RenderComponent>();
  mCylinderEntity = std::get<0>(entity);

  auto& renderComponent = std::get<2>(entity);
  renderComponent.mMesh = basalt::get_renderer()->add_mesh(
    vertices.data(), static_cast<i32>(vertices.size()), vertexLayout,
    PrimitiveType::TriangleStrip
  );
  renderComponent.mDiffuseColor = Color(255, 255, 0);
  renderComponent.mAmbientColor = Color(255, 255, 0);
  renderComponent.mRenderFlags = RenderFlagCullNone;
}

void Lights::on_show() {
  set_current_scene(mScene);
}

void Lights::on_hide() {}

void Lights::on_update() {
  const auto deltaTime = static_cast<f32>(basalt::get_delta_time());
  const auto radOffetX = PI * 0.5f * deltaTime;
  auto& transform =
    mScene->get_entity_registry().get<TransformComponent>(mCylinderEntity);
  transform.rotate(radOffetX, 0.0f, 0.0f);

  auto* renderer = basalt::get_renderer();

  LightSetup lights;
  lights.set_global_ambient_color(Color(32, 32, 32));

  mLightAngle += PI * 0.25f * deltaTime;
  // reset when rotated 360°
  while (mLightAngle >= PI * 2.0f) {
    mLightAngle -= PI * 2.0f;
  }

  const Vec3f32 lightDir(
    std::cosf(mLightAngle), 1.0f, std::sinf(mLightAngle)
  );
  lights.add_directional_light(
    Vec3f32::normalize(lightDir), Color(255, 255, 255)
  );

  renderer->set_lights(lights);
}

} // namespace d3d9_tuts
