#include "sandbox/d3d9/matrices.h"

#include "sandbox/d3d9/utils.h"

#include <runtime/Engine.h>
#include <runtime/Prelude.h>

#include <runtime/gfx/types.h>

#include <runtime/gfx/backend/IRenderer.h>
#include <runtime/gfx/backend/Types.h>

#include <runtime/scene/transform.h>
#include <runtime/math/Constants.h>

#include <entt/entity/registry.hpp>

#include <array>

using std::array;
using namespace std::string_view_literals;

using basalt::Transform;
using basalt::math::PI;
using basalt::gfx::RenderComponent;
using basalt::gfx::backend::IRenderer;
using basalt::gfx::backend::RenderFlagCullNone;
using basalt::gfx::backend::RenderFlagDisableLighting;
using basalt::gfx::backend::VertexElement;
using basalt::gfx::backend::VertexLayout;

namespace d3d9 {

Matrices::Matrices(IRenderer* const renderer) {
  mScene->set_background_color(Color {0.0f, 0.0f, 0.0f});

  struct Vertex final {
    f32 x;
    f32 y;
    f32 z;
    ColorEncoding::A8R8G8B8 color;
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

  auto& ecs = mScene->ecs();
  mTriangleEntity = ecs.create();
  ecs.assign<Transform>(mTriangleEntity);
  auto& rc = ecs.assign<RenderComponent>(mTriangleEntity);

  rc.mMesh = add_triangle_list_mesh(renderer, vertices, vertexLayout);
  rc.mRenderFlags = RenderFlagCullNone | RenderFlagDisableLighting;
}

void Matrices::on_show() {
  basalt::set_view({mScene, create_default_camera()});
}

void Matrices::on_hide() {
}

void Matrices::on_update(const f64 deltaTime) {
  // 1 full rotation per second
  const auto radOffsetY = 2.0f * PI * static_cast<f32>(deltaTime);
  auto& transform = mScene->ecs().get<Transform>(mTriangleEntity);

  transform.rotate(0.0f, radOffsetY, 0.0f);
}

auto Matrices::name() -> std::string_view {
  return "Tutorial 3: Using Matrices"sv;
}

} // namespace d3d9
