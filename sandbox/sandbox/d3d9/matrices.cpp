#include "matrices.h"

#include "utils.h"

#include <api/debug.h>
#include <api/prelude.h>

#include <api/gfx/draw_target.h>
#include <api/scene/transform.h>
#include <api/math/constants.h>
#include <api/shared/config.h>

#include <entt/entity/registry.hpp>

#include <array>

using std::array;
using std::string_view;
using namespace std::literals;

using basalt::Debug;
using basalt::PI;
using basalt::Transform;
using basalt::gfx::Device;
using basalt::gfx::RenderComponent;
using basalt::gfx::RenderFlagCullNone;
using basalt::gfx::RenderFlagDisableLighting;
using basalt::gfx::SceneView;
using basalt::gfx::VertexElement;
using basalt::gfx::VertexLayout;

namespace d3d9 {

Matrices::Matrices(Device& device) {
  mScene->set_background_color(Colors::BLACK);

  struct Vertex final {
    f32 x {};
    f32 y {};
    f32 z {};
    ColorEncoding::A8R8G8B8 color {};
  };

  array<Vertex, 3u> vertices {
    Vertex {-1.0f, -1.0f, 0.0f,
            ColorEncoding::pack_logical_a8r8g8b8(255, 0, 0)},
    Vertex {1.0f, -1.0f, 0.0f, ColorEncoding::pack_logical_a8r8g8b8(0, 0, 255)},
    Vertex {0.0f, 1.0f, 0.0f,
            ColorEncoding::pack_logical_a8r8g8b8(255, 255, 255)}};

  const VertexLayout vertexLayout {VertexElement::Position3F32,
                                   VertexElement::ColorDiffuse1U32};

  entt::registry& ecs {mScene->ecs()};
  mTriangle = ecs.create();
  ecs.emplace<Transform>(mTriangle);

  auto& rc {ecs.emplace<RenderComponent>(mTriangle)};
  rc.mesh = add_triangle_list_mesh(device, vertices, vertexLayout);
  rc.renderFlags = RenderFlagCullNone | RenderFlagDisableLighting;

  mSceneView = std::make_shared<SceneView>(mScene, create_default_camera());
}

void Matrices::on_update(const basalt::UpdateContext& ctx) {
  // 1 full rotation per second
  const f32 radOffsetY {2.0f * PI * static_cast<f32>(ctx.deltaTime)};
  auto& transform {mScene->ecs().get<Transform>(mTriangle)};
  transform.rotate(0.0f, radOffsetY, 0.0f);

  ctx.drawTarget.draw(mSceneView);

  if (ctx.engine.config().debugUiEnabled) {
    Debug::update(*mScene);
  }
}

auto Matrices::name() -> string_view {
  return "Tutorial 3: Using Matrices"sv;
}

} // namespace d3d9
