#include "Lights.h"

#include <runtime/Prelude.h>

#include <runtime/Engine.h>

#include <runtime/gfx/Camera.h>
#include <runtime/gfx/RenderComponent.h>

#include <runtime/gfx/backend/IRenderer.h>
#include <runtime/gfx/backend/Types.h>

#include <runtime/math/Constants.h>
#include <runtime/math/Vec3.h>

#include <entt/entity/registry.hpp>

#include <array>
#include <cmath>
#include <tuple>

using std::array;

using basalt::TransformComponent;
using basalt::math::PI;
using basalt::math::Vec3f32;
using basalt::gfx::Camera;
using basalt::gfx::RenderComponent;
using basalt::gfx::backend::LightSetup;
using basalt::gfx::backend::PrimitiveType;
using basalt::gfx::backend::RenderFlagCullNone;
using basalt::gfx::backend::VertexElement;
using basalt::gfx::backend::VertexLayout;

namespace d3d9_tuts {

Lights::Lights() {
  mScene->set_background_color(Color {0.0f, 0.0f, 1.0f});

  const Vec3f32 cameraPos(0.0f, 3.0f, -5.0f);
  const Vec3f32 lookAt(0.0f, 0.0f, 0.0f);
  const Vec3f32 up(0.0f, 1.0f, 0.0f);
  mScene->set_camera(Camera(cameraPos, lookAt, up));

  struct Vertex final {
    Vec3f32 pos;
    Vec3f32 normal;
  };

  array<Vertex, 50u * 2> vertices;
  for (uSize i = 0u; i < 50u; i++) {
    const auto theta = (2.0f * PI * i) / (50 - 1);
    const auto sinTheta = std::sinf(theta);
    const auto cosTheta = std::cosf(theta);
    vertices[2 * i].pos = {sinTheta, -1.0f, cosTheta};
    vertices[2 * i].normal = {sinTheta, 0.0f, cosTheta};
    vertices[2 * i + 1].pos = {sinTheta, 1.0f, cosTheta};
    vertices[2 * i + 1].normal = {sinTheta, 0.0f, cosTheta};
  }

  const VertexLayout vertexLayout {
    VertexElement::Position3F32, VertexElement::Normal3F32
  };

  auto& entityRegistry = mScene->get_entity_registry();
  auto entity = entityRegistry.create<TransformComponent, RenderComponent>();
  mCylinderEntity = std::get<0>(entity);

  auto& renderComponent = std::get<2>(entity);
  renderComponent.mMesh =
    basalt::get_renderer()->add_mesh(vertices.data(),
                                     static_cast<i32>(vertices.size()),
                                     vertexLayout,
                                     PrimitiveType::TriangleStrip);
  renderComponent.mDiffuseColor = Color(1.0f, 1.0f, 0.0f);
  renderComponent.mAmbientColor = Color(1.0f, 1.0f, 0.0f);
  renderComponent.mRenderFlags = RenderFlagCullNone;
}

void Lights::on_show() {
  set_current_scene(mScene);
}

void Lights::on_hide() {}

void Lights::on_update() {
  const auto deltaTime = static_cast<f32>(basalt::get_delta_time());
  const auto radOffsetX = PI * 0.5f * deltaTime;
  auto& transform =
    mScene->get_entity_registry().get<TransformComponent>(mCylinderEntity);
  transform.rotate(radOffsetX, 0.0f, 0.0f);

  auto* renderer = basalt::get_renderer();

  LightSetup lights;
  lights.set_global_ambient_color(Color::from_rgba(32, 32, 32));

  mLightAngle += PI * 0.25f * deltaTime;
  // reset when rotated 360°
  while (mLightAngle >= PI * 2.0f) {
    mLightAngle -= PI * 2.0f;
  }

  const Vec3f32 lightDir(std::cosf(mLightAngle), 1.0f, std::sinf(mLightAngle));
  lights.add_directional_light(Vec3f32::normalize(lightDir),
                               Color(1.0f, 1.0f, 1.0f));

  renderer->set_lights(lights);
}

} // namespace d3d9_tuts
