#include "vertices.h"

#include "utils.h"

#include <api/engine.h>
#include <api/prelude.h>

#include <api/gfx/command_list_recorder.h>
#include <api/gfx/drawable.h>
#include <api/gfx/draw_target.h>
#include <api/gfx/backend/types.h>

#include <array>
#include <memory>

using std::array;
using std::string_view;
using namespace std::literals;

using basalt::Engine;
using basalt::Size2Du16;
using basalt::gfx::CommandList;
using basalt::gfx::CommandListRecorder;
using basalt::gfx::Drawable;
using basalt::gfx::MeshHandle;
using basalt::gfx::ResourceCache;
using basalt::gfx::VertexElement;
using basalt::gfx::VertexLayout;

namespace d3d9 {

namespace {

struct MyDrawable final : Drawable {
  explicit MyDrawable(const MeshHandle triangle) noexcept
    : mTriangle {triangle} {
  }

  auto draw(ResourceCache&, Size2Du16) -> CommandList override {
    CommandListRecorder cmdListRecorder;
    cmdListRecorder.draw(mTriangle);

    return cmdListRecorder.complete_command_list();
  }

  [[nodiscard]] auto clear_color() const -> std::optional<Color> override {
    return Colors::BLUE;
  }

private:
  MeshHandle mTriangle {MeshHandle::null()};
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

  array<Vertex, 3u> vertices {
    Vertex {150.0f, 50.0f, 0.5f, 1.0f,
            ColorEncoding::pack_logical_a8r8g8b8(255, 0, 0)},
    Vertex {250.0f, 250.0f, 0.5f, 1.0f,
            ColorEncoding::pack_logical_a8r8g8b8(0, 255, 0)},
    Vertex {50.0f, 250.0f, 0.5f, 1.0f,
            ColorEncoding::pack_logical_a8r8g8b8(0, 255, 255)}};

  const VertexLayout vertexLayout {VertexElement::PositionTransformed4F32,
                                   VertexElement::ColorDiffuse1U32};

  const MeshHandle triangle =
    add_triangle_list_mesh(*engine.gfx_device(), vertices, vertexLayout);
  mDrawable = std::make_shared<MyDrawable>(triangle);
}

void Vertices::on_update(const basalt::UpdateContext& ctx) {
  ctx.drawTarget.draw(mDrawable);
}

auto Vertices::name() -> string_view {
  return "Tutorial 2: Rendering Vertices"sv;
}

} // namespace d3d9
