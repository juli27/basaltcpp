#include "sandbox/d3d9/textures.h"

#include "sandbox/d3d9/utils.h"

#include <runtime/prelude.h>

#include <runtime/scene/transform.h>

#include <runtime/math/Constants.h>
#include <runtime/math/Vec2.h>
#include <runtime/math/Vec3.h>

#include <array>
#include <cmath>

using std::array;
using std::string_view;
using namespace std::literals;

using basalt::Transform;
using basalt::gfx::RenderComponent;
using basalt::gfx::SceneView;
using basalt::gfx::backend::IRenderer;
using basalt::gfx::backend::RenderFlagCullNone;
using basalt::gfx::backend::RenderFlagDisableLighting;
using basalt::gfx::backend::VertexElement;
using basalt::gfx::backend::VertexLayout;
using basalt::math::PI;
using basalt::math::Vec2f32;
using basalt::math::Vec3f32;

namespace d3d9 {

Textures::Textures(IRenderer* const renderer) {
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
  rc.mMesh = add_triangle_strip_mesh(renderer, vertices, vertexLayout);
  rc.mTexture = renderer->add_texture("data/banana.bmp");
  rc.mRenderFlags = RenderFlagCullNone | RenderFlagDisableLighting;
}

auto Textures::view(const basalt::Size2Du16 windowSize) -> SceneView {
  return SceneView {mScene, create_default_camera(windowSize)};
}

void Textures::on_update(const f64 deltaTime) {
  auto& transform {mScene->ecs().get<Transform>(mCylinder)};
  transform.rotate(static_cast<f32>(deltaTime), 0.0f, 0.0f);
}

auto Textures::name() -> string_view {
  return "Tutorial 5: Using Texture Maps"sv;
}

} // namespace d3d9
