#include "sandbox/d3d9/textures_tci.h"

#include "sandbox/d3d9/utils.h"

#include <runtime/debug.h>
#include <runtime/prelude.h>

#include <runtime/gfx/Camera.h>
#include <runtime/scene/transform.h>

#include <runtime/math/Constants.h>
#include <runtime/math/Mat4.h>
#include <runtime/math/Vec3.h>

#include <array>
#include <cmath>

using std::array;
using std::string_view;
using namespace std::literals;

using basalt::Debug;
using basalt::Transform;
using basalt::gfx::Camera;
using basalt::gfx::RenderComponent;
using basalt::gfx::SceneView;
using basalt::gfx::backend::IRenderer;
using basalt::gfx::backend::RenderFlagCullNone;
using basalt::gfx::backend::RenderFlagDisableLighting;
using basalt::gfx::backend::TexCoordinateSrc;
using basalt::gfx::backend::VertexElement;
using basalt::gfx::backend::VertexLayout;
using basalt::math::Mat4f32;
using basalt::math::PI;
using basalt::math::Vec3f32;

namespace d3d9 {

TexturesTci::TexturesTci(
  IRenderer& renderer, const basalt::Size2Du16 windowSize) {
  mScene->set_background_color(Colors::BLUE);

  struct Vertex final {
    Vec3f32 pos {};
    ColorEncoding::A8R8G8B8 color {};
  };

  array<Vertex, 50u * 2> vertices {};
  for (uSize i = 0u; i < 50u; i++) {
    const f32 theta {2.0f * PI * i / (50 - 1)};
    const f32 sinTheta {std::sin(theta)};
    const f32 cosTheta {std::cos(theta)};

    vertices[2 * i].pos = Vec3f32 {sinTheta, -1.0f, cosTheta};
    vertices[2 * i].color = ColorEncoding::pack_logical_a8r8g8b8(255, 255, 255);

    vertices[2 * i + 1].pos = Vec3f32 {sinTheta, 1.0f, cosTheta};
    vertices[2 * i + 1].color = ColorEncoding::pack_logical_a8r8g8b8(
      128, 128, 128);
  }

  const VertexLayout vertexLayout {
    VertexElement::Position3F32, VertexElement::ColorDiffuse1U32
  };

  entt::registry& ecs {mScene->ecs()};
  mCylinder = ecs.create();
  ecs.emplace<Transform>(mCylinder);

  auto& rc {ecs.emplace<RenderComponent>(mCylinder)};
  rc.mMesh = add_triangle_strip_mesh(renderer, vertices, vertexLayout);
  rc.mTexture = renderer.add_texture("data/banana.bmp");
  rc.mRenderFlags = RenderFlagCullNone | RenderFlagDisableLighting;

  // TODO: fix jitter
  const Camera camera {create_default_camera()};
  rc.texTransform = camera.projection_matrix(windowSize) * Mat4f32::scaling(
    {0.5f, -0.5f, 1.0f}) * Mat4f32::translation({0.5f, 0.5f, 0.0f});
  rc.tcs = TexCoordinateSrc::PositionCameraSpace;

  mSceneView = std::make_shared<SceneView>(mScene, create_default_camera());
}

void TexturesTci::on_update(const basalt::UpdateContext& ctx) {
  auto& transform {mScene->ecs().get<Transform>(mCylinder)};
  transform.rotate(static_cast<f32>(ctx.deltaTime), 0.0f, 0.0f);

  ctx.drawTarget.draw(mSceneView);

  if (ctx.engine.config().debugUiEnabled) {
    Debug::update(*mScene);
  }
}

auto TexturesTci::name() -> string_view {
  return "Tutorial 5: Using Texture Maps (TCI)"sv;
}

} // namespace d3d9
