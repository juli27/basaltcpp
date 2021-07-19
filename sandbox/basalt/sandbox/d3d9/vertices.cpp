#include <basalt/sandbox/d3d9/vertices.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/command_list_recorder.h>
#include <basalt/api/gfx/drawable.h>
#include <basalt/api/gfx/surface.h>
#include <basalt/api/gfx/backend/types.h>

#include <gsl/span>

#include <array>
#include <memory>

using std::array;
using std::string_view;
using namespace std::literals;

using basalt::Engine;
using basalt::RectangleU16;
using basalt::Size2Du16;
using basalt::gfx::CommandList;
using basalt::gfx::CommandListRecorder;
using basalt::gfx::Drawable;
using basalt::gfx::Mesh;
using basalt::gfx::MeshDescriptor;
using basalt::gfx::PrimitiveType;
using basalt::gfx::ResourceCache;
using basalt::gfx::VertexElement;
using basalt::gfx::VertexLayout;

namespace d3d9 {

namespace {

struct MyDrawable final : Drawable {
  explicit MyDrawable(const Mesh triangle) noexcept : mTriangle {triangle} {
  }

  auto draw(ResourceCache& cache, const Size2Du16 viewport, const RectangleU16&)
    -> std::tuple<CommandList, RectangleU16> override {
    CommandListRecorder cmdList;
    cmdList.clear(Colors::BLUE);

    const auto& mesh = cache.get(mTriangle);
    cmdList.draw(mesh.vertexBuffer, mesh.primitiveType,
                         mesh.startVertex, mesh.primitiveCount);

    return {cmdList.take_cmd_list(), viewport.to_rectangle()};
  }

private:
  Mesh mTriangle {Mesh::null()};
};

} // namespace

Vertices::Vertices(Engine& engine) {
  struct Vertex final {
    f32 x;
    f32 y;
    f32 z;
    f32 rhw;
    ColorEncoding::A8R8G8B8 color;
  };

  const array<Vertex, 3u> vertices {
    Vertex {150.0f, 50.0f, 0.5f, 1.0f,
            ColorEncoding::pack_logical_a8r8g8b8(255, 0, 0)},
    Vertex {250.0f, 250.0f, 0.5f, 1.0f,
            ColorEncoding::pack_logical_a8r8g8b8(0, 255, 0)},
    Vertex {50.0f, 250.0f, 0.5f, 1.0f,
            ColorEncoding::pack_logical_a8r8g8b8(0, 255, 255)}};

  const VertexLayout vertexLayout {VertexElement::PositionTransformed4F32,
                                   VertexElement::ColorDiffuse1U32};

  const MeshDescriptor mesh {as_bytes(gsl::span {vertices}), vertexLayout,
                             PrimitiveType::TriangleList,
                             static_cast<u32>(vertices.size() / 3)};

  const Mesh triangle = engine.gfx_resource_cache().create_mesh(mesh);
  mDrawable = std::make_shared<MyDrawable>(triangle);
}

void Vertices::on_update(const basalt::UpdateContext& ctx) {
  ctx.drawTarget.draw(mDrawable);
}

auto Vertices::name() -> string_view {
  return "Tutorial 2: Rendering Vertices"sv;
}

} // namespace d3d9
