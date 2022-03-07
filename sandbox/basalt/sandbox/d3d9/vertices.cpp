#include <basalt/sandbox/d3d9/vertices.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/drawable.h>

#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/shared/size2d.h>

#include <gsl/span>

#include <algorithm>
#include <array>
#include <memory>
#include <utility>

using std::array;

using basalt::Engine;
using basalt::RectangleU16;
using basalt::Size2Du16;
using basalt::gfx::Attachment;
using basalt::gfx::Attachments;
using basalt::gfx::CommandList;
using basalt::gfx::Drawable;
using basalt::gfx::Pipeline;
using basalt::gfx::PipelineDescriptor;
using basalt::gfx::PrimitiveType;
using basalt::gfx::ResourceCache;
using basalt::gfx::VertexBuffer;
using basalt::gfx::VertexBufferDescriptor;
using basalt::gfx::VertexElement;
using basalt::gfx::VertexLayout;

namespace d3d9 {

namespace {

struct MyDrawable final : Drawable {
  explicit MyDrawable(Engine& engine) noexcept
    : mResourceCache {engine.gfx_resource_cache()} {
    mPipeline = mResourceCache.create_pipeline(
      PipelineDescriptor {PrimitiveType::TriangleList});

    struct Vertex final {
      f32 x;
      f32 y;
      f32 z;
      f32 rhw;
      ColorEncoding::A8R8G8B8 color;
    };

    const array vertices {
      Vertex {150.0f, 50.0f, 0.5f, 1.0f,
              ColorEncoding::pack_a8r8g8b8_u32(255, 0, 0)},
      Vertex {250.0f, 250.0f, 0.5f, 1.0f,
              ColorEncoding::pack_a8r8g8b8_u32(0, 255, 0)},
      Vertex {50.0f, 250.0f, 0.5f, 1.0f,
              ColorEncoding::pack_a8r8g8b8_u32(0, 255, 255)}};

    const auto vertexData {as_bytes(gsl::span {vertices})};

    const VertexBufferDescriptor vertexBufferDesc {
      vertexData.size_bytes(),
      VertexLayout {
        VertexElement::PositionTransformed4F32,
        VertexElement::ColorDiffuse1U32A8R8G8B8,
      },
    };
    mVertexBuffer = mResourceCache.create_vertex_buffer(vertexBufferDesc);
    mResourceCache.with_mapping_of(
      mVertexBuffer, [&](const gsl::span<std::byte> mapping) {
        std::copy_n(vertexData.begin(), mapping.size_bytes(), mapping.begin());
      });
  }

  MyDrawable(const MyDrawable&) = delete;
  MyDrawable(MyDrawable&&) noexcept = default;

  ~MyDrawable() noexcept override {
    mResourceCache.destroy(mVertexBuffer);
    mResourceCache.destroy(mPipeline);
  }

  auto operator=(const MyDrawable&) -> MyDrawable& = delete;
  auto operator=(MyDrawable&&) -> MyDrawable& = delete;

  auto draw(ResourceCache&, const Size2Du16 viewport, const RectangleU16&)
    -> std::tuple<CommandList, RectangleU16> override {
    CommandList cmdList {};

    cmdList.clear_attachments(Attachments {Attachment::Color}, Colors::BLUE,
                              1.0f, 0);

    cmdList.bind_pipeline(mPipeline);
    cmdList.bind_vertex_buffer(mVertexBuffer, 0ull);
    cmdList.draw(0, 3);

    return {std::move(cmdList), viewport.to_rectangle()};
  }

private:
  ResourceCache& mResourceCache;
  Pipeline mPipeline {Pipeline::null()};
  VertexBuffer mVertexBuffer {VertexBuffer::null()};
};

} // namespace

Vertices::Vertices(Engine& engine)
  : mDrawable {std::make_shared<MyDrawable>(engine)} {
}

auto Vertices::drawable() -> basalt::gfx::DrawablePtr {
  return mDrawable;
}

void Vertices::tick(Engine&) {
}

} // namespace d3d9
