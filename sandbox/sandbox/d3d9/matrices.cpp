#include "sandbox/d3d9/matrices.h"

#include "sandbox/d3d9/utils.h"

#include <runtime/Engine.h>
#include <runtime/prelude.h>

#include <runtime/scene/transform.h>
#include <runtime/math/Constants.h>

#include <entt/entity/registry.hpp>

#include <array>

using std::array;
using std::string_view;
using namespace std::literals;

using basalt::Transform;
using basalt::gfx::RenderComponent;
using basalt::gfx::View;
using basalt::gfx::backend::IRenderer;
using basalt::gfx::backend::RenderFlagCullNone;
using basalt::gfx::backend::RenderFlagDisableLighting;
using basalt::gfx::backend::VertexElement;
using basalt::gfx::backend::VertexLayout;
using basalt::math::PI;

namespace d3d9 {

Matrices::Matrices(IRenderer* const renderer) {
  mScene->set_background_color(Color {0.0f, 0.0f, 0.0f});

  struct Vertex final {
    f32 x {};
    f32 y {};
    f32 z {};
    ColorEncoding::A8R8G8B8 color {};
  };

  array<Vertex, 3u> vertices {
    Vertex {
      -1.0f, -1.0f, 0.0f, ColorEncoding::pack_logical_a8r8g8b8(255, 0, 0)
    }
  , Vertex {
      1.0f, -1.0f, 0.0f, ColorEncoding::pack_logical_a8r8g8b8(0, 0, 255)
    }
  , Vertex {
      0.0f, 1.0f, 0.0f, ColorEncoding::pack_logical_a8r8g8b8(255, 255, 255)
    }
  };

  const VertexLayout vertexLayout {
    VertexElement::Position3F32, VertexElement::ColorDiffuse1U32
  };

  entt::registry& ecs {mScene->ecs()};
  mTriangle = ecs.create();
  ecs.emplace<Transform>(mTriangle);

  auto& rc {ecs.emplace<RenderComponent>(mTriangle)};
  rc.mMesh = add_triangle_list_mesh(renderer, vertices, vertexLayout);
  rc.mRenderFlags = RenderFlagCullNone | RenderFlagDisableLighting;
}

void Matrices::on_show() {
  basalt::set_view(View {mScene, create_default_camera()});
}

void Matrices::on_hide() {
}

void Matrices::on_update(const f64 deltaTime) {
  // 1 full rotation per second
  const f32 radOffsetY {2.0f * PI * static_cast<f32>(deltaTime)};
  auto& transform {mScene->ecs().get<Transform>(mTriangle)};
  transform.rotate(0.0f, radOffsetY, 0.0f);
}

auto Matrices::name() -> string_view {
  return "Tutorial 3: Using Matrices"sv;
}

} // namespace d3d9
