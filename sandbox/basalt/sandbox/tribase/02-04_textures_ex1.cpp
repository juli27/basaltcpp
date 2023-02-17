#include <basalt/sandbox/tribase/02-04_textures_ex1.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/resource_cache.h>

#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/matrix4x4.h>
#include <basalt/api/math/vector3.h>

#include <gsl/span>

#include <array>
#include <string_view>
#include <utility>

using namespace std::literals;

using std::array;

using gsl::span;

using namespace basalt::literals;

using basalt::Engine;
using basalt::Matrix4x4f32;
using basalt::Vector3f32;
using basalt::gfx::Attachment;
using basalt::gfx::Attachments;
using basalt::gfx::CommandList;
using basalt::gfx::PipelineDescriptor;
using basalt::gfx::PrimitiveType;
using basalt::gfx::TextureBlendingStage;
using basalt::gfx::TransformState;
using basalt::gfx::VertexElement;

namespace tribase {

TexturesEx1::TexturesEx1(Engine& engine)
  : mGfxCache {engine.gfx_resource_cache()}
  , mTexture {mGfxCache.load_texture("data/banana.bmp"sv)} {
  struct Vertex final {
    f32 x;
    f32 y;
    f32 z;
    f32 u;
    f32 v;
  };

  const array vertices {
    Vertex {0.0f, 0.5f, 0.0f, 0.5f, 0.0f},
    Vertex {0.5f, -0.5f, 0.0f, 1.0f, 1.0f},
    Vertex {-0.5f, -0.5f, 0.0f, 0.0f, 1.0f},
  };

  const span vertexData {as_bytes(span {vertices})};

  mVertexBuffer = mGfxCache.create_vertex_buffer(
    {
      vertexData.size(),
      {VertexElement::Position3F32, VertexElement::TextureCoords2F32},
    },
    vertexData);

  array vertexInputState {
    VertexElement::Position3F32,
    VertexElement::TextureCoords2F32,
  };

  TextureBlendingStage textureStage {};

  PipelineDescriptor pipelineDesc;
  pipelineDesc.vertexInputState = vertexInputState;
  pipelineDesc.textureStages = span {&textureStage, 1};
  pipelineDesc.primitiveType = PrimitiveType::TriangleList;

  mPipeline = mGfxCache.create_pipeline(pipelineDesc);

  mSampler = mGfxCache.create_sampler({});
}

TexturesEx1::~TexturesEx1() noexcept {
  mGfxCache.destroy(mSampler);
  mGfxCache.destroy(mPipeline);
  mGfxCache.destroy(mVertexBuffer);
  mGfxCache.destroy(mTexture);
}

auto TexturesEx1::on_draw(const DrawContext& drawContext) -> void {
  CommandList cmdList;
  cmdList.clear_attachments(Attachments {Attachment::RenderTarget},
                            Color::from_non_linear(0.103f, 0.103f, 0.103f),
                            1.0f, 0u);

  cmdList.bind_pipeline(mPipeline);
  cmdList.bind_vertex_buffer(mVertexBuffer, 0);

  const f32 aspectRatio {static_cast<f32>(drawContext.viewport.width()) /
                         static_cast<f32>(drawContext.viewport.height())};
  cmdList.set_transform(
    TransformState::ViewToViewport,
    Matrix4x4f32::perspective_projection(90.0_deg, aspectRatio, 0.1f, 100.0f));
  cmdList.set_transform(TransformState::WorldToView, Matrix4x4f32::identity());
  cmdList.set_transform(
    TransformState::ModelToWorld,
    Matrix4x4f32::translation(Vector3f32 {0.0f, 0.0f, 1.0f}));

  cmdList.bind_sampler(mSampler);
  cmdList.bind_texture(mTexture);

  cmdList.draw(0, 3);

  drawContext.commandLists.emplace_back(std::move(cmdList));
}

} // namespace tribase
