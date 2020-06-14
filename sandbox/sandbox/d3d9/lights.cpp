#include "lights.h"

#include "utils.h"

#include <runtime/debug.h>
#include <runtime/prelude.h>

#include <runtime/gfx/draw_target.h>
#include <runtime/scene/transform.h>

#include <runtime/math/constants.h>
#include <runtime/math/vec3.h>

#include <runtime/shared/config.h>

#include <array>
#include <cmath>

using std::array;
using std::string_view;
using namespace std::literals;

using basalt::Debug;
using basalt::PI;
using basalt::Transform;
using basalt::Vec3f32;
using basalt::gfx::Device;
using basalt::gfx::RenderComponent;
using basalt::gfx::RenderFlagCullNone;
using basalt::gfx::SceneView;
using basalt::gfx::VertexElement;
using basalt::gfx::VertexLayout;

namespace d3d9 {

Lights::Lights(Device& device) {
  mScene->set_background_color(Colors::BLUE);
  mScene->set_ambient_light(Color::from_rgba(32, 32, 32));

  struct Vertex final {
    Vec3f32 pos {};
    Vec3f32 normal {};
  };

  array<Vertex, 50u * 2u> vertices {};
  for (uSize i = 0u; i < 50u; i++) {
    const f32 theta {2.0f * PI * i / (50 - 1)};
    const f32 sinTheta {std::sin(theta)};
    const f32 cosTheta {std::cos(theta)};
    vertices[2 * i].pos.set(sinTheta, -1.0f, cosTheta);
    vertices[2 * i].normal.set(sinTheta, 0.0f, cosTheta);
    vertices[2 * i + 1].pos.set(sinTheta, 1.0f, cosTheta);
    vertices[2 * i + 1].normal.set(sinTheta, 0.0f, cosTheta);
  }

  const VertexLayout vertexLayout {
    VertexElement::Position3F32, VertexElement::Normal3F32
  };

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

  const Vec3f32 lightDir {std::cos(mLightAngle), 1.0f, std::sin(mLightAngle)};
  mScene->add_directional_light(Vec3f32::normalize(lightDir), Colors::WHITE);

  ctx.drawTarget.draw(mSceneView);

  if (ctx.engine.config().debugUiEnabled) {
      Debug::update(*mScene);
  }
}

auto Lights::name() -> string_view {
  return "Tutorial 4: Creating and Using Lights"sv;
}

} // namespace d3d9
