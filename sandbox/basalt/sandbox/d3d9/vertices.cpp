#include <basalt/sandbox/d3d9/vertices.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/backend/command_list.h>

#include <gsl/span>

#include <algorithm>
#include <array>
#include <cstddef>

using std::array;
using std::byte;

using gsl::span;

using basalt::Engine;
using basalt::gfx::Attachment;
using basalt::gfx::Attachments;
using basalt::gfx::CommandList;
using basalt::gfx::PipelineDescriptor;
using basalt::gfx::PrimitiveType;
using basalt::gfx::VertexBufferDescriptor;
using basalt::gfx::VertexElement;
using basalt::gfx::VertexLayout;

namespace d3d9 {

Vertices::Vertices(Engine& engine)
  : mResourceCache {engine.gfx_resource_cache()} {
  PipelineDescriptor pipelineDesc {};
  pipelineDesc.primitiveType = PrimitiveType::TriangleList;
  mPipeline = mResourceCache.create_pipeline(pipelineDesc);

  struct Vertex final {
    f32 x;
    f32 y;
    f32 z;
    f32 rhw;
    ColorEncoding::A8R8G8B8 color;
  };

  const array vertices {Vertex {150.0f, 50.0f, 0.5f, 1.0f,
                                ColorEncoding::pack_a8r8g8b8_u32(255, 0, 0)},
                        Vertex {250.0f, 250.0f, 0.5f, 1.0f,
                                ColorEncoding::pack_a8r8g8b8_u32(0, 255, 0)},
                        Vertex {50.0f, 250.0f, 0.5f, 1.0f,
                                ColorEncoding::pack_a8r8g8b8_u32(0, 255, 255)}};

  const auto vertexData {as_bytes(span {vertices})};

  const VertexBufferDescriptor vertexBufferDesc {
    vertexData.size_bytes(),
    VertexLayout {
      VertexElement::PositionTransformed4F32,
      VertexElement::ColorDiffuse1U32A8R8G8B8,
    },
  };
  mVertexBuffer = mResourceCache.create_vertex_buffer(vertexBufferDesc);
  mResourceCache.with_mapping_of(mVertexBuffer, [&](const span<byte> mapping) {
    std::copy_n(vertexData.begin(), mapping.size_bytes(), mapping.begin());
  });
}

Vertices::~Vertices() noexcept {
  mResourceCache.destroy(mVertexBuffer);
  mResourceCache.destroy(mPipeline);
}

auto Vertices::on_draw(const DrawContext&) -> CommandList {
  CommandList cmdList {};

  cmdList.clear_attachments(Attachments {Attachment::Color}, Colors::BLUE, 1.0f,
                            0);

  cmdList.bind_pipeline(mPipeline);
  cmdList.bind_vertex_buffer(mVertexBuffer, 0ull);
  cmdList.draw(0, 3);

  return cmdList;
}

} // namespace d3d9
