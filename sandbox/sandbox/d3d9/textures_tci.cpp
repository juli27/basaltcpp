#include "textures_tci.h"

#include "utils.h"

#include <api/debug.h>
#include <api/prelude.h>

#include <api/gfx/camera.h>
#include <api/gfx/draw_target.h>

#include <api/gfx/backend/device.h>
#include <api/gfx/backend/context.h>

#include <api/scene/transform.h>

#include <api/math/constants.h>
#include <api/math/mat4.h>

#include <api/shared/config.h>

#include <array>
#include <cmath>

using std::array;
using std::string_view;
using namespace std::literals;

using basalt::Debug;
using basalt::Engine;
using basalt::Mat4f32;
using basalt::PI;
using basalt::Transform;
using basalt::gfx::Camera;
using basalt::gfx::RenderComponent;
using basalt::gfx::RenderFlagCullNone;
using basalt::gfx::RenderFlagDisableLighting;
using basalt::gfx::SceneView;
using basalt::gfx::TexCoordinateSrc;
using basalt::gfx::VertexElement;
using basalt::gfx::VertexLayout;

namespace d3d9 {

TexturesTci::TexturesTci(Engine& engine) {
  mScene->set_background_color(Colors::BLUE);

  struct Vertex final {
    f32 x {};
    f32 y {};
    f32 z {};
    ColorEncoding::A8R8G8B8 color {};
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

    auto& vertex2 = vertices[2 * i + 1];
    vertex2.x = sinTheta;
    vertex2.y = 1.0f;
    vertex2.z = cosTheta;
    vertex2.color = ColorEncoding::pack_logical_a8r8g8b8(128, 128, 128);
  }

  const VertexLayout vertexLayout {
    VertexElement::Position3F32, VertexElement::ColorDiffuse1U32
  };

  entt::registry& ecs {mScene->ecs()};
  mCylinder = ecs.create();
  ecs.emplace<Transform>(mCylinder);

  auto& rc {ecs.emplace<RenderComponent>(mCylinder)};
  auto& device = engine.gfx_device();
  rc.mesh = add_triangle_strip_mesh(device, vertices, vertexLayout);
  rc.texture = device.add_texture("data/banana.bmp");
  rc.renderFlags = RenderFlagCullNone | RenderFlagDisableLighting;

  // TODO: fix jitter
  const Camera camera {create_default_camera()};
  rc.texTransform = camera.projection_matrix(
    engine.gfx_context().surface_size()) * Mat4f32::scaling(
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
