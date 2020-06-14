#include "textures.h"

#include "utils.h"

#include <runtime/debug.h>
#include <runtime/prelude.h>

#include <runtime/gfx/draw_target.h>
#include <runtime/scene/transform.h>

#include <runtime/math/constants.h>
#include <runtime/math/vec2.h>
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
using basalt::Vec2f32;
using basalt::Vec3f32;
using basalt::gfx::Device;
using basalt::gfx::RenderComponent;
using basalt::gfx::RenderFlagCullNone;
using basalt::gfx::RenderFlagDisableLighting;
using basalt::gfx::SceneView;
using basalt::gfx::VertexElement;
using basalt::gfx::VertexLayout;

namespace d3d9 {

Textures::Textures(Device& device) {
  mScene->set_background_color(Colors::BLUE);

  struct Vertex final {
    Vec3f32 pos {};
    ColorEncoding::A8R8G8B8 color {};
    Vec2f32 texCoords {};
  };

  array<Vertex, 50u * 2> vertices {};

  for (uSize i = 0u; i < 50u; i++) {
    const f32 theta {2.0f * PI * i / (50 - 1)};
    const f32 sinTheta {std::sin(theta)};
    const f32 cosTheta {std::cos(theta)};

    vertices[2 * i].pos.set(sinTheta, -1.0f, cosTheta);
    vertices[2 * i].color = ColorEncoding::pack_logical_a8r8g8b8(255, 255, 255);
    vertices[2 * i].texCoords.set(i / (50.0f - 1), 1.0f);

    vertices[2 * i + 1].pos.set(sinTheta, 1.0f, cosTheta);
    vertices[2 * i + 1].color = ColorEncoding::pack_logical_a8r8g8b8(
      128, 128, 128);
    vertices[2 * i + 1].texCoords.set(i / (50.0f - 1), 0.0f);
  }

  const VertexLayout vertexLayout {
    VertexElement::Position3F32, VertexElement::ColorDiffuse1U32
  , VertexElement::TextureCoords2F32
  };

  entt::registry& ecs {mScene->ecs()};
  mCylinder = ecs.create();
  ecs.emplace<Transform>(mCylinder);

  auto& rc {ecs.emplace<RenderComponent>(mCylinder)};
  rc.mesh = add_triangle_strip_mesh(device, vertices, vertexLayout);
  rc.texture = device.add_texture("data/banana.bmp");
  rc.renderFlags = RenderFlagCullNone | RenderFlagDisableLighting;

  mSceneView = std::make_shared<SceneView>(mScene, create_default_camera());
}

void Textures::on_update(const basalt::UpdateContext& ctx) {
  auto& transform {mScene->ecs().get<Transform>(mCylinder)};
  transform.rotate(static_cast<f32>(ctx.deltaTime), 0.0f, 0.0f);

  ctx.drawTarget.draw(mSceneView);

  if (ctx.engine.config().debugUiEnabled) {
      Debug::update(*mScene);
  }
}

auto Textures::name() -> string_view {
  return "Tutorial 5: Using Texture Maps"sv;
}

} // namespace d3d9
