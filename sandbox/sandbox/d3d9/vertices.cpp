#include "sandbox/d3d9/vertices.h"

#include "sandbox/d3d9/utils.h"

#include <runtime/Prelude.h>

#include <runtime/Engine.h>

#include <runtime/gfx/types.h> // RenderComponent

#include <runtime/gfx/backend/IRenderer.h>
#include <runtime/gfx/backend/Types.h>

#include <array>

using std::array;
using namespace std::string_view_literals;

using basalt::gfx::RenderComponent;
using basalt::gfx::backend::IRenderer;
using basalt::gfx::backend::VertexElement;
using basalt::gfx::backend::VertexLayout;

namespace d3d9 {

Vertices::Vertices(IRenderer* const renderer) {
  mScene->set_background_color(Color {0.0f, 0.0f, 1.0f});

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

  const auto [entity, renderComp] = mScene->create_entity<RenderComponent>();
  renderComp.mMesh = add_triangle_list_mesh(renderer, vertices, vertexLayout);
}

void Vertices::on_show() {
  basalt::set_view({mScene});
}

void Vertices::on_hide() {
}

void Vertices::on_update(const f64) {
}

auto Vertices::name() -> std::string_view {
  return "Tutorial 2: Rendering Vertices"sv;
}

} // namespace d3d9
