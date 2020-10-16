#include "textures.h"

#include "utils.h"

#include <api/debug.h>
#include <api/engine.h>
#include <api/prelude.h>

#include <api/gfx/draw_target.h>

#include <api/scene/transform.h>

#include <api/resources/types.h>

#include <api/math/constants.h>

#include <api/shared/config.h>

#include <array>
#include <cmath>

using std::array;
using std::string_view;
using namespace std::literals;

using basalt::Debug;
using basalt::Engine;
using basalt::MaterialDescriptor;
using basalt::PI;
using basalt::Texture;
using basalt::Transform;
using basalt::gfx::Device;
using basalt::gfx::RenderComponent;
using basalt::gfx::SceneView;
using basalt::gfx::VertexElement;
using basalt::gfx::VertexLayout;

namespace d3d9 {

Textures::Textures(Engine& engine) {
  mScene->set_background(Colors::BLUE);

  struct Vertex final {
    f32 x {};
    f32 y {};
    f32 z {};
    ColorEncoding::A8R8G8B8 color {};
    f32 u {};
    f32 v {};
  };

  array<Vertex, 50u * 2> vertices {};

  for (uSize i = 0u; i < 50u; i++) {
    const f32 theta {2.0f * PI * i / (50 - 1)};
    const f32 sinTheta {std::sin(theta)};
    const f32 cosTheta {std::cos(theta)};

    auto& vertex1 = vertices[2 * i];
    vertex1.x = sinTheta;
    vertex1.y = -1.0f;
    vertex1.z = cosTheta;
    vertex1.color = ColorEncoding::pack_logical_a8r8g8b8(255, 255, 255);
    vertex1.u = i / (50.0f - 1);
    vertex1.v = 1.0f;

    auto& vertex2 = vertices[2 * i + 1];
    vertex2.x = sinTheta;
    vertex2.y = 1.0f;
    vertex2.z = cosTheta;
    vertex2.color = ColorEncoding::pack_logical_a8r8g8b8(128, 128, 128);
    vertex2.u = i / (50.0f - 1);
    vertex2.v = 0.0f;
  }

  const VertexLayout vertexLayout {VertexElement::Position3F32,
                                   VertexElement::ColorDiffuse1U32,
                                   VertexElement::TextureCoords2F32};

  entt::registry& ecs {mScene->ecs()};
  mCylinder = ecs.create();
  ecs.emplace<Transform>(mCylinder);

  auto& rc {ecs.emplace<RenderComponent>(mCylinder)};
  const auto device = engine.gfx_device();
  rc.mesh = add_triangle_strip_mesh(*device, vertices, vertexLayout);
  rc.texture = engine.load<Texture>("data/banana.bmp"sv);

  MaterialDescriptor material;
  material.cullBackFace = false;
  material.lit = false;

  rc.material = engine.load(material);

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
