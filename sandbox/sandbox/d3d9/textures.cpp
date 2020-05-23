#include "sandbox/d3d9/textures.h"

#include "sandbox/d3d9/utils.h"

#include <runtime/Engine.h>
#include <runtime/Prelude.h>

#include <runtime/gfx/types.h>

#include <runtime/gfx/backend/IRenderer.h>
#include <runtime/gfx/backend/Types.h>

#include <runtime/scene/transform.h>

#include <runtime/math/Constants.h>
#include <runtime/math/Vec2.h>
#include <runtime/math/Vec3.h>

#include <entt/entity/registry.hpp>

#include <array>
#include <cmath>
#include <tuple>

using std::array;
using std::string_view;
using namespace std::string_view_literals;

using basalt::Transform;
using basalt::math::PI;
using basalt::math::Vec2f32;
using basalt::math::Vec3f32;
using basalt::gfx::RenderComponent;
using basalt::gfx::backend::IRenderer;
using basalt::gfx::backend::RenderFlagCullNone;
using basalt::gfx::backend::RenderFlagDisableLighting;
using basalt::gfx::backend::VertexElement;
using basalt::gfx::backend::VertexLayout;

namespace d3d9 {

Textures::Textures(IRenderer* const renderer) {
  mScene->set_background_color(Color {0.0f, 0.0f, 1.0f});

  struct Vertex final {
    Vec3f32 pos {};
    ColorEncoding::A8R8G8B8 color {};
    Vec2f32 texCoords {};
  };

  array<Vertex, 50u * 2> vertices {};

  for (uSize i = 0u; i < 50u; i++) {
    const auto theta = (2.0f * PI * i) / (50 - 1);
    const auto sinTheta = std::sin(theta);
    const auto cosTheta = std::cos(theta);

    vertices[2 * i].pos = {sinTheta, -1.0f, cosTheta};
    vertices[2 * i].color = ColorEncoding::pack_logical_a8r8g8b8(255, 255, 255);
    vertices[2 * i].texCoords = {i / (50.0f - 1), 1.0f};

    vertices[2 * i + 1].pos = {sinTheta, 1.0f, cosTheta};
    vertices[2 * i + 1].color = ColorEncoding::pack_logical_a8r8g8b8(
      128, 128, 128);
    vertices[2 * i + 1].texCoords = {i / (50.0f - 1), 0.0f};
  }

  const VertexLayout vertexLayout {
    VertexElement::Position3F32, VertexElement::ColorDiffuse1U32
  , VertexElement::TextureCoords2F32
  };

  auto& ecs = mScene->ecs();
  mCylinderEntity = ecs.create();
  ecs.assign<Transform>(mCylinderEntity);

  auto& rc = ecs.assign<RenderComponent>(mCylinderEntity);
  rc.mMesh = add_triangle_strip_mesh(renderer, vertices, vertexLayout);
  rc.mTexture = renderer->add_texture("data/banana.bmp");
  rc.mRenderFlags = RenderFlagCullNone | RenderFlagDisableLighting;
}

void Textures::on_show() {
  basalt::set_view({mScene, create_default_camera()});
}

void Textures::on_hide() {
}

void Textures::on_update(const f64 deltaTime) {
  auto& transform = mScene->ecs().get<Transform>(mCylinderEntity);
  transform.rotate(static_cast<f32>(deltaTime), 0.0f, 0.0f);
}

auto Textures::name() -> string_view {
  return "Tutorial 5: Using Texture Maps"sv;
}

} // namespace d3d9
