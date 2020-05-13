#include "sandbox/d3d9_tutorials/Matrices.h"

#include "sandbox/d3d9/utils.h"

#include <runtime/Engine.h>
#include <runtime/platform/Platform.h>
#include <runtime/Prelude.h>

#include <runtime/gfx/Camera.h>
#include <runtime/gfx/RenderComponent.h>

#include <runtime/gfx/backend/IRenderer.h>
#include <runtime/gfx/backend/RenderCommand.h>
#include <runtime/gfx/backend/Types.h>

#include <runtime/math/Constants.h>
#include <runtime/math/Mat4.h>
#include <runtime/math/Vec3.h>

#include <entt/entity/registry.hpp>

#include <array>

using std::array;

using basalt::TransformComponent;
using basalt::math::Mat4f32;
using basalt::math::PI;
using basalt::math::Vec3f32;
using basalt::gfx::Camera;
using basalt::gfx::RenderComponent;
using basalt::gfx::backend::IRenderer;
using basalt::gfx::backend::RenderFlagCullNone;
using basalt::gfx::backend::RenderFlagDisableLighting;
using basalt::gfx::backend::VertexElement;
using basalt::gfx::backend::VertexLayout;

namespace d3d9 {

Matrices::Matrices(IRenderer* const renderer) {
  mScene->set_background_color(Color {0.0f, 0.0f, 0.0f});

  const Vec3f32 cameraPos {0.0f, 3.0f, -5.0f};
  const Vec3f32 lookAt {0.0f, 0.0f, 0.0f};
  const Vec3f32 up {0.0f, 1.0f, 0.0f};
  const auto windowSize = basalt::platform::get_window_size();
  const auto aspectRatio {
    static_cast<f32>(windowSize.width()) / static_cast<f32>(windowSize.height())
  };
  const auto projection = Mat4f32::perspective_projection(
    PI / 4.0f, aspectRatio, 1.0f, 100.0f);
  mScene->set_camera(Camera {cameraPos, lookAt, up, projection});

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

  const auto [entity, transform, rc] =
    mScene->create_entity<TransformComponent, RenderComponent>();
  mTriangleEntity = entity;

  rc.mMesh = add_triangle_list_mesh(renderer, vertices, vertexLayout);
  rc.mRenderFlags = RenderFlagCullNone | RenderFlagDisableLighting;
}

void Matrices::on_show() {
  set_current_scene(mScene);
}

void Matrices::on_hide() {
}

void Matrices::on_update(const f64 deltaTime) {
  // 1 full rotation per second
  const auto radOffsetY = 2.0f * PI * static_cast<f32>(deltaTime);
  auto& transform = mScene->get_entity_registry().get<TransformComponent>(
    mTriangleEntity);

  transform.rotate(0.0f, radOffsetY, 0.0f);

  mScene->display_debug_gui();
}

} // namespace d3d9
