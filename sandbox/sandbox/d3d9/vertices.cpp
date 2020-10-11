#include "vertices.h"

#include "utils.h"

#include <api/debug.h>
#include <api/engine.h>
#include <api/prelude.h>

#include <api/gfx/draw_target.h>
#include <api/shared/config.h>

#include <array>

using std::array;
using std::string_view;
using namespace std::literals;

using basalt::Debug;
using basalt::gfx::Device;
using basalt::gfx::RenderComponent;
using basalt::gfx::SceneView;
using basalt::gfx::VertexElement;
using basalt::gfx::VertexLayout;

namespace d3d9 {

Vertices::Vertices(Device& device) {
  mScene->set_background(Colors::BLUE);

  struct Vertex final {
    f32 x;
    f32 y;
    f32 z;
    f32 rhw;
    ColorEncoding::A8R8G8B8 color;
  };

  array<Vertex, 3u> vertices {
    Vertex {150.0f, 50.0f, 0.5f, 1.0f,
            ColorEncoding::pack_logical_a8r8g8b8(255, 0, 0)},
    Vertex {250.0f, 250.0f, 0.5f, 1.0f,
            ColorEncoding::pack_logical_a8r8g8b8(0, 255, 0)},
    Vertex {50.0f, 250.0f, 0.5f, 1.0f,
            ColorEncoding::pack_logical_a8r8g8b8(0, 255, 255)}};

  const VertexLayout vertexLayout {VertexElement::PositionTransformed4F32,
                                   VertexElement::ColorDiffuse1U32};

  entt::registry& ecs {mScene->ecs()};
  const entt::entity entity {ecs.create()};
  auto& rc {ecs.emplace<RenderComponent>(entity)};
  rc.mesh = add_triangle_list_mesh(device, vertices, vertexLayout);

  mSceneView = std::make_shared<SceneView>(mScene, create_default_camera());
}

void Vertices::on_update(const basalt::UpdateContext& ctx) {
  ctx.drawTarget.draw(mSceneView);

  if (ctx.engine.config().debugUiEnabled) {
    Debug::update(*mScene);
  }
}

auto Vertices::name() -> string_view {
  return "Tutorial 2: Rendering Vertices"sv;
}

} // namespace d3d9
