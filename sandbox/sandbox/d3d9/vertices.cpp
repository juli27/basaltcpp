#include "sandbox/d3d9/vertices.h"

#include "sandbox/d3d9/utils.h"

#include <runtime/Engine.h>
#include <runtime/prelude.h>

#include <array>

using std::array;
using std::string_view;
using namespace std::literals;

using basalt::gfx::RenderComponent;
using basalt::gfx::View;
using basalt::gfx::backend::IRenderer;
using basalt::gfx::backend::VertexElement;
using basalt::gfx::backend::VertexLayout;

namespace d3d9 {

Vertices::Vertices(IRenderer* const renderer) {
  mScene->set_background_color(Colors::BLUE);

  struct Vertex final {
    f32 x;
    f32 y;
    f32 z;
    f32 rhw;
    ColorEncoding::A8R8G8B8 color;
  };

  array<Vertex, 3u> vertices {
    Vertex {
      150.0f, 50.0f, 0.5f, 1.0f
    , ColorEncoding::pack_logical_a8r8g8b8(255, 0, 0)
    }
  , Vertex {
      250.0f, 250.0f, 0.5f, 1.0f
    , ColorEncoding::pack_logical_a8r8g8b8(0, 255, 0)
    }
  , Vertex {
      50.0f, 250.0f, 0.5f, 1.0f
    , ColorEncoding::pack_logical_a8r8g8b8(0, 255, 255)
    }
  };

  const VertexLayout vertexLayout {
    VertexElement::PositionTransformed4F32, VertexElement::ColorDiffuse1U32
  };

  entt::registry& ecs {mScene->ecs()};
  const entt::entity entity {ecs.create()};
  auto& rc {ecs.emplace<RenderComponent>(entity)};
  rc.mMesh = add_triangle_list_mesh(renderer, vertices, vertexLayout);
}

void Vertices::on_show(const basalt::Size2Du16) {
  basalt::set_view(View {mScene});
}

void Vertices::on_hide() {
}

void Vertices::on_update(const f64) {
}

auto Vertices::name() -> string_view {
  return "Tutorial 2: Rendering Vertices"sv;
}

} // namespace d3d9
