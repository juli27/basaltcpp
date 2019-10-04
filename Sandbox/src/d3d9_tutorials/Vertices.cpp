#include "Vertices.h"

#include <basalt/Prelude.h> // SetCurrentScene, gfx

#include <array>
#include <memory> // make_shared
#include <tuple> // get

using std::array;

using bs::Color;
using bs::Scene;
using bs::TransformComponent;
using bs::gfx::RenderComponent;
using bs::gfx::backend::PrimitiveType;
using bs::gfx::backend::VertexElementType;
using bs::gfx::backend::VertexElementUsage;
using bs::gfx::backend::VertexLayout;

namespace d3d9_tuts {

Vertices::Vertices() : mScene(std::make_shared<Scene>()) {
  mScene->set_background_color(Color(0, 0, 255));

  struct Vertex final {
    f32 x, y, z, rhw;
    u32 color;
  };

  array<Vertex, 3u> vertices = {{
    {150.0f,  50.0f, 0.5f, 1.0f, Color(255, 0  , 0).to_argb()},
    {250.0f, 250.0f, 0.5f, 1.0f, Color(0  , 255, 0).to_argb()},
    { 50.0f, 250.0f, 0.5f, 1.0f, Color(0  , 255, 255).to_argb()}
  }};

  const VertexLayout vertexLayout{
    { VertexElementUsage::PositionTransformed, VertexElementType::F32_4 },
    { VertexElementUsage::ColorDiffuse, VertexElementType::U32_1 }
  };

  auto& entityRegistry = mScene->get_entity_registry();
  const auto triangleEntity = entityRegistry.create<TransformComponent, RenderComponent>();
  std::get<2>(triangleEntity).mMesh = bs::get_renderer()->add_mesh(
    vertices.data(), static_cast<i32>(vertices.size()), vertexLayout,
    PrimitiveType::TriangleList
  );
}

void Vertices::on_show() {
  bs::set_current_scene(mScene);
}


void Vertices::on_hide() {}


void Vertices::on_update() {}

} // namespace d3d9_tuts
