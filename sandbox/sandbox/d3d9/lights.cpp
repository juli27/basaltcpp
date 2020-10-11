#include "lights.h"

#include "utils.h"

#include <api/debug.h>
#include <api/engine.h>
#include <api/prelude.h>

#include <api/gfx/draw_target.h>
#include <api/scene/transform.h>

#include <api/math/constants.h>
#include <api/math/vector3.h>

#include <api/shared/config.h>

#include <array>
#include <cmath>

using std::array;
using std::string_view;
using namespace std::literals;

using basalt::Debug;
using basalt::PI;
using basalt::Transform;
using basalt::Vector3f32;
using basalt::gfx::Device;
using basalt::gfx::RenderComponent;
using basalt::gfx::RenderFlagCullNone;
using basalt::gfx::SceneView;
using basalt::gfx::VertexElement;
using basalt::gfx::VertexLayout;

namespace d3d9 {

Lights::Lights(Device& device) {
  mScene->set_background(Colors::BLUE);
  mScene->set_ambient_light(Color::from_rgba(32, 32, 32));

  struct Vertex final {
    f32 x {};
    f32 y {};
    f32 z {};
    f32 nx {};
    f32 ny {};
    f32 nz {};
  };

  array<Vertex, 50u * 2u> vertices {};
  for (uSize i = 0u; i < 50u; i++) {
    const f32 theta {2.0f * PI * i / (50 - 1)};
    const f32 sinTheta {std::sin(theta)};
    const f32 cosTheta {std::cos(theta)};

    auto& vertex1 = vertices[2 * i];
    vertex1.x = sinTheta;
    vertex1.y = -1.0f;
    vertex1.z = cosTheta;
    vertex1.nx = sinTheta;
    vertex1.nz = cosTheta;

    auto& vertex2 = vertices[2 * i + 1];
    vertex2.x = sinTheta;
    vertex2.y = 1.0f;
    vertex2.z = cosTheta;
    vertex2.nx = sinTheta;
    vertex2.nz = cosTheta;
  }

  const VertexLayout vertexLayout {VertexElement::Position3F32,
                                   VertexElement::Normal3F32};

  entt::registry& ecs {mScene->ecs()};
  mCylinder = ecs.create();
  ecs.emplace<Transform>(mCylinder);

  auto& rc {ecs.emplace<RenderComponent>(mCylinder)};
  rc.mesh = add_triangle_strip_mesh(device, vertices, vertexLayout);
  rc.diffuseColor = Color {1.0f, 1.0f, 0.0f};
  rc.ambientColor = Color {1.0f, 1.0f, 0.0f};
  rc.renderFlags = RenderFlagCullNone;

  mSceneView = std::make_shared<SceneView>(mScene, create_default_camera());
}

void Lights::on_update(const basalt::UpdateContext& ctx) {
  const auto dt {static_cast<f32>(ctx.deltaTime)};
  auto& transform {mScene->ecs().get<Transform>(mCylinder)};
  transform.rotate(2.0f * dt, 0.0f, 0.0f);

  mLightAngle += 20.0f / 7.0f * dt;
  // reset when rotated 360°
  while (mLightAngle >= PI * 2.0f) {
    mLightAngle -= PI * 2.0f;
  }

  mScene->clear_directional_lights();

  const Vector3f32 lightDir {std::cos(mLightAngle), 1.0f,
                             std::sin(mLightAngle)};
  mScene->add_directional_light(Vector3f32::normalize(lightDir), Colors::WHITE);

  ctx.drawTarget.draw(mSceneView);

  if (ctx.engine.config().debugUiEnabled) {
    Debug::update(*mScene);
  }
}

auto Lights::name() -> string_view {
  return "Tutorial 4: Creating and Using Lights"sv;
}

} // namespace d3d9
