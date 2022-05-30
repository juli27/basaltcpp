#include <basalt/sandbox/d3d9/matrices.h>

#include <basalt/sandbox/d3d9/utils.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/resource_cache.h>

#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/math/constants.h>
#include <basalt/api/math/matrix4x4.h>

#include <gsl/span>

#include <algorithm>
#include <array>
#include <cstddef>
#include <utility>

using std::array;
using std::byte;

using gsl::span;

using basalt::Angle;
using basalt::Engine;
using basalt::Matrix4x4f32;
using basalt::PI;
using basalt::gfx::Attachment;
using basalt::gfx::Attachments;
using basalt::gfx::CommandList;
using basalt::gfx::PipelineDescriptor;
using basalt::gfx::PrimitiveType;
using basalt::gfx::TransformState;
using basalt::gfx::VertexBufferDescriptor;
using basalt::gfx::VertexElement;
using basalt::gfx::VertexLayout;

namespace d3d9 {

Matrices::Matrices(Engine& engine)
  : mResourceCache {engine.gfx_resource_cache()}
  , mCamera {create_default_camera()} {
  constexpr array<const VertexElement, 2> vertexLayout {
    VertexElement::Position3F32,
    VertexElement::ColorDiffuse1U32A8R8G8B8,
  };

  PipelineDescriptor pipelineDesc {};
  pipelineDesc.vertexInputState = vertexLayout;
  pipelineDesc.primitiveType = PrimitiveType::TriangleList;
  mPipeline = mResourceCache.create_pipeline(pipelineDesc);

  struct Vertex final {
    f32 x;
    f32 y;
    f32 z;
    ColorEncoding::A8R8G8B8 color;
  };

  const array vertices {
    Vertex {-1.0f, -1.0f, 0.0f, ColorEncoding::pack_a8r8g8b8_u32(255, 0, 0)},
    Vertex {1.0f, -1.0f, 0.0f, ColorEncoding::pack_a8r8g8b8_u32(0, 0, 255)},
    Vertex {0.0f, 1.0f, 0.0f, ColorEncoding::pack_a8r8g8b8_u32(255, 255, 255)},
  };

  const auto vertexData {as_bytes(span {vertices})};

  const VertexBufferDescriptor vertexBufferDesc {
    vertexData.size_bytes(),
    VertexLayout {
      VertexElement::Position3F32,
      VertexElement::ColorDiffuse1U32A8R8G8B8,
    },
  };
  mVertexBuffer = mResourceCache.create_vertex_buffer(vertexBufferDesc);
  mResourceCache.with_mapping_of(mVertexBuffer, [&](const span<byte> mapping) {
    std::copy_n(vertexData.begin(), mapping.size_bytes(), mapping.begin());
  });
}

Matrices::~Matrices() noexcept {
  mResourceCache.destroy(mVertexBuffer);
  mResourceCache.destroy(mPipeline);
}

auto Matrices::on_draw(const DrawContext& context) -> void {
  CommandList cmdList {};

  cmdList.clear_attachments(Attachments {Attachment::RenderTarget},
                            Colors::BLACK, 1.0f, 0);

  cmdList.bind_pipeline(mPipeline);

  cmdList.set_transform(TransformState::ModelToWorld,
                        Matrix4x4f32::rotation_y(mRotationY));
  cmdList.set_transform(TransformState::WorldToView, mCamera.world_to_view());
  cmdList.set_transform(TransformState::ViewToViewport,
                        mCamera.view_to_viewport(context.viewport));

  cmdList.bind_vertex_buffer(mVertexBuffer, 0ull);

  cmdList.draw(0, 3);

  context.commandLists.push_back(std::move(cmdList));
}

void Matrices::on_tick(Engine& engine) {
  const auto dt {static_cast<f32>(engine.delta_time())};

  constexpr f32 twoPi {PI * 2.0f};
  // 1 full rotation per second
  mRotationY += Angle::radians(twoPi * dt);
  while (mRotationY.radians() > PI) {
    mRotationY -= Angle::radians(twoPi);
  }
}

} // namespace d3d9
