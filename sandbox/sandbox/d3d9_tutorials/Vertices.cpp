#include "Vertices.h"

#include <runtime/Prelude.h>

#include <runtime/Engine.h> // get_renderer

#include <runtime/gfx/RenderComponent.h> // RenderComponent

#include <runtime/gfx/backend/IRenderer.h>
#include <runtime/gfx/backend/Types.h>

#include <entt/entity/registry.hpp>

#include <array>
#include <tuple>

using std::array;

using basalt::TransformComponent;
using basalt::gfx::RenderComponent;
using basalt::gfx::backend::PrimitiveType;
using basalt::gfx::backend::VertexElementType;
using basalt::gfx::backend::VertexElementUsage;
using basalt::gfx::backend::VertexLayout;

namespace d3d9_tuts {

Vertices::Vertices() {
  mScene->set_background_color(Color {0.0f, 0.0f, 1.0f});

  struct Vertex final {
    f32 x, y, z, rhw;
    Pixels::A8R8G8B8 color;
  };

  array<Vertex, 3u> vertices {
    Vertex {150.0f, 50.0f, 0.5f, 1.0f, Pixels::pack_logical_a8_r8_g8_b8(255, 0, 0)},
    Vertex {250.0f, 250.0f, 0.5f, 1.0f, Pixels::pack_logical_a8_r8_g8_b8(0, 255, 0)},
    Vertex {50.0f, 250.0f, 0.5f, 1.0f, Pixels::pack_logical_a8_r8_g8_b8(0, 255, 255)}
  };

  const VertexLayout vertexLayout {
    {VertexElementType::F32_4, VertexElementUsage::PositionTransformed},
    {VertexElementType::U32_1, VertexElementUsage::ColorDiffuse}
  };

  auto& entityRegistry = mScene->get_entity_registry();
  const auto triangleEntity = entityRegistry.create<
    TransformComponent, RenderComponent>();

  std::get<2>(triangleEntity).mMesh = basalt::get_renderer()->add_mesh(
    vertices.data(), static_cast<i32>(vertices.size()), vertexLayout,
    PrimitiveType::TriangleList
  );
}

void Vertices::on_show() {
  set_current_scene(mScene);
}

void Vertices::on_hide() {
}

void Vertices::on_update() {
}

} // namespace d3d9_tuts
