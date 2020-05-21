#include "sandbox/d3d9/lights.h"

#include "sandbox/d3d9/utils.h"

#include <runtime/Engine.h>
#include <runtime/Prelude.h>

#include <runtime/gfx/types.h>

#include <runtime/gfx/backend/Types.h>

#include <runtime/math/Constants.h>
#include <runtime/math/Vec3.h>

#include <entt/entity/registry.hpp>

#include <array>
#include <cmath>
#include <tuple>

using std::array;
using namespace std::string_view_literals;

using basalt::TransformComponent;
using basalt::math::PI;
using basalt::math::Vec3f32;
using basalt::gfx::RenderComponent;
using basalt::gfx::backend::IRenderer;
using basalt::gfx::backend::RenderFlagCullNone;
using basalt::gfx::backend::VertexElement;
using basalt::gfx::backend::VertexLayout;

namespace d3d9 {

Lights::Lights(IRenderer* const renderer) {
  mScene->set_background_color(Color {0.0f, 0.0f, 1.0f});
  mScene->set_ambient_light(Color::from_rgba(32, 32, 32));

  struct Vertex final {
    Vec3f32 pos;
    Vec3f32 normal;
  };

  array<Vertex, 50u * 2u> vertices {};
  for (uSize i = 0u; i < 50u; i++) {
    const auto theta = (2.0f * PI * i) / (50 - 1);
    const auto sinTheta = std::sin(theta);
    const auto cosTheta = std::cos(theta);
    vertices[2 * i].pos.set(sinTheta, -1.0f, cosTheta);
    vertices[2 * i].normal.set(sinTheta, 0.0f, cosTheta);
    vertices[2 * i + 1].pos.set(sinTheta, 1.0f, cosTheta);
    vertices[2 * i + 1].normal.set(sinTheta, 0.0f, cosTheta);
  }

  const VertexLayout vertexLayout {
    VertexElement::Position3F32, VertexElement::Normal3F32
  };

  const auto [entity, transform, rc] =
    mScene->create_entity<TransformComponent, RenderComponent>();
  mCylinderEntity = entity;

  rc.mMesh = add_triangle_strip_mesh(renderer, vertices, vertexLayout);
  rc.mDiffuseColor = Color {1.0f, 1.0f, 0.0f};
  rc.mAmbientColor = Color {1.0f, 1.0f, 0.0f};
  rc.mRenderFlags = RenderFlagCullNone;
}

void Lights::on_show() {
  basalt::set_view({mScene, create_default_camera()});
}

void Lights::on_hide() {
}

void Lights::on_update(const f64 deltaTime) {
  // TODO: fix rotation speed
  const auto radOffsetX = PI * 0.5f * static_cast<f32>(deltaTime);
  auto& transform = mScene->get_entity_registry().get<TransformComponent>(
    mCylinderEntity);
  transform.rotate(radOffsetX, 0.0f, 0.0f);

  // TODO: fix rotation speed
  mLightAngle += PI * 0.25f * static_cast<f32>(deltaTime);
  // reset when rotated 360°
  while (mLightAngle >= PI * 2.0f) {
    mLightAngle -= PI * 2.0f;
  }

  mScene->clear_directional_lights();

  const Vec3f32 lightDir {std::cos(mLightAngle), 1.0f, std::sin(mLightAngle)};
  mScene->add_directional_light(Vec3f32::normalize(lightDir), Color {1.0f, 1.0f, 1.0f});
}

auto Lights::name() -> std::string_view {
  return "Tutorial 4: Creating and Using Lights"sv;
}

} // namespace d3d9
