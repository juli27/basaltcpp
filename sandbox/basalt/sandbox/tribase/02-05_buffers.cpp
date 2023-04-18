#include <basalt/sandbox/tribase/02-05_buffers.h>

#include <basalt/api/engine.h>
#include <basalt/api/input_events.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/math/matrix4x4.h>

#include <gsl/span>

#include <array>
#include <cstddef>
#include <random>
#include <utility>

using namespace basalt::literals;

using basalt::Angle;
using basalt::Engine;
using basalt::InputEvent;
using basalt::InputEventHandled;
using basalt::Key;
using basalt::Matrix4x4f32;
using basalt::Vector3f32;
using basalt::gfx::Attachment;
using basalt::gfx::Attachments;
using basalt::gfx::CommandList;
using basalt::gfx::IndexBufferDescriptor;
using basalt::gfx::Pipeline;
using basalt::gfx::PipelineDescriptor;
using basalt::gfx::PrimitiveType;
using basalt::gfx::SamplerDescriptor;
using basalt::gfx::TestOp;
using basalt::gfx::TextureBlendingStage;
using basalt::gfx::TextureFilter;
using basalt::gfx::TextureMipFilter;
using basalt::gfx::TextureOp;
using basalt::gfx::TextureStageArgument;
using basalt::gfx::TransformState;
using basalt::gfx::VertexBufferDescriptor;
using basalt::gfx::VertexElement;

using gsl::span;

using std::array;
using std::byte;
using std::default_random_engine;
using std::random_device;
using std::uniform_real_distribution;

namespace tribase {

namespace {

using Distribution = uniform_real_distribution<float>;

struct Vertex final {
  Vector3f32 pos;
  ColorEncoding::A8R8G8B8 diffuse;
  f32 u;
  f32 v;
};

constexpr u32 NUM_CUBES {2048};
constexpr u32 NUM_TRIANGLES_PER_CUBE {2 * 6};
constexpr u32 NUM_VERTICES_PER_CUBE {8};
constexpr u32 NUM_INDICES_PER_CUBE {NUM_TRIANGLES_PER_CUBE * 3};

static_assert(NUM_CUBES * NUM_VERTICES_PER_CUBE <= 0xffff,
              "can't index vertices with u16");

auto fill_buffers(const span<Vertex> vb, const span<u16> ib) {
  default_random_engine randomEngine {random_device {}()};
  Distribution rng1 {-1.0f, 1.0f};
  Distribution rng2 {20.0f, 250.f};
  Distribution rng3 {0.0f, 1.0f};

  const auto normalizedRandomVector {[&] {
    return Vector3f32::normalize(Vector3f32 {
      rng1(randomEngine),
      rng1(randomEngine),
      rng1(randomEngine),
    });
  }};

  const array<u16, NUM_INDICES_PER_CUBE> cubeIndices {
    0, 3, 7, 0, 7, 4, // front
    2, 1, 5, 2, 5, 6, // back
    1, 0, 4, 1, 4, 5, // left
    3, 2, 6, 3, 6, 7, // right
    0, 1, 2, 0, 2, 3, // top
    6, 5, 4, 6, 4, 7}; // bottom

  for (u32 iCube {0}; iCube < NUM_CUBES; iCube++) {
    const Vector3f32 pos {normalizedRandomVector() * rng2(randomEngine)};

    const u16 startVertex {static_cast<u16>(NUM_VERTICES_PER_CUBE * iCube)};

    vb[startVertex + 0].pos = pos + Vector3f32 {-1.0f, 1.0f, -1.0f};
    vb[startVertex + 1].pos = pos + Vector3f32 {-1.0f, 1.0f, 1.0f};
    vb[startVertex + 2].pos = pos + Vector3f32 {1.0f, 1.0f, 1.0f};
    vb[startVertex + 3].pos = pos + Vector3f32 {1.0f, 1.0f, -1.0f};
    vb[startVertex + 4].pos = pos + Vector3f32 {-1.0f, -1.0f, -1.0f};
    vb[startVertex + 5].pos = pos + Vector3f32 {-1.0f, -1.0f, 1.0f};
    vb[startVertex + 6].pos = pos + Vector3f32 {1.0f, -1.0f, 1.0f};
    vb[startVertex + 7].pos = pos + Vector3f32 {1.0f, -1.0f, -1.0f};

    for (Vertex& vertex : vb.subspan(startVertex, NUM_VERTICES_PER_CUBE)) {
      const Color vertexColor {Color::from_non_linear(rng3(randomEngine),
                                                      rng3(randomEngine),
                                                      rng3(randomEngine)) *
                               2.0f};
      vertex.diffuse = vertexColor.to_argb();

      vertex.u = rng1(randomEngine);
      vertex.v = rng1(randomEngine);
    }

    const u32 startIndex {NUM_INDICES_PER_CUBE * iCube};

    for (u32 iIndex {0}; iIndex < NUM_INDICES_PER_CUBE; iIndex++) {
      // add startIndex as offset to every index
      ib[startIndex + iIndex] = cubeIndices[iIndex] + startVertex;
    }
  }
}

} // namespace

Buffers::Buffers(Engine& engine)
  : mResourceCache {engine.gfx_resource_cache()}, mFov {90_deg} {
  constexpr array vertexLayout {
    VertexElement::Position3F32,
    VertexElement::ColorDiffuse1U32A8R8G8B8,
    VertexElement::TextureCoords2F32,
  };

  TextureBlendingStage textureStage;
  textureStage.arg1 = TextureStageArgument::SampledTexture;
  textureStage.arg2 = TextureStageArgument::Diffuse;
  textureStage.colorOp = TextureOp::Modulate;
  textureStage.alphaOp = TextureOp::SelectArg1;

  PipelineDescriptor pipelineDesc;
  pipelineDesc.vertexInputState = vertexLayout;
  pipelineDesc.textureStages = span {&textureStage, 1};
  pipelineDesc.primitiveType = PrimitiveType::TriangleList;
  pipelineDesc.depthTest = TestOp::PassIfLessEqual;
  pipelineDesc.depthWriteEnable = true;
  pipelineDesc.dithering = true;
  mPipeline = mResourceCache.create_pipeline(pipelineDesc);

  VertexBufferDescriptor vbDesc;
  vbDesc.layout = vertexLayout;
  vbDesc.sizeInBytes = NUM_VERTICES_PER_CUBE * sizeof(Vertex) * NUM_CUBES;
  mVertexBuffer = mResourceCache.create_vertex_buffer(vbDesc);

  IndexBufferDescriptor ibDesc;
  ibDesc.sizeInBytes = NUM_INDICES_PER_CUBE * sizeof(u16) * NUM_CUBES;
  mIndexBuffer = mResourceCache.create_index_buffer(ibDesc);

  mResourceCache.with_mapping_of(mVertexBuffer, [this](
                                                  const span<byte> vbData) {
    span vb {reinterpret_cast<Vertex*>(vbData.data()),
             vbData.size() / sizeof(Vertex)};

    mResourceCache.with_mapping_of(mIndexBuffer, [vb](const span<byte> ibData) {
      span ib {reinterpret_cast<u16*>(ibData.data()),
               ibData.size() / sizeof(u16)};
      fill_buffers(vb, ib);
    });
  });

  SamplerDescriptor samplerDesc {};
  samplerDesc.magFilter = TextureFilter::Bilinear;
  samplerDesc.minFilter = TextureFilter::Bilinear;
  samplerDesc.mipFilter = TextureMipFilter::Linear;
  mSampler = mResourceCache.create_sampler(samplerDesc);

  mTexture = mResourceCache.load_texture("data/tribase/Texture2.bmp");
}

Buffers::~Buffers() noexcept {
  mResourceCache.destroy(mTexture);
  mResourceCache.destroy(mSampler);
  mResourceCache.destroy(mIndexBuffer);
  mResourceCache.destroy(mVertexBuffer);
  mResourceCache.destroy(mPipeline);
}

auto Buffers::on_draw(const DrawContext& drawContext) -> void {
  CommandList cmdList;

  cmdList.clear_attachments(
    Attachments {Attachment::RenderTarget, Attachment::DepthBuffer},
    Colors::BLACK, 1.0f, 0);

  cmdList.bind_pipeline(mPipeline);

  cmdList.bind_sampler(mSampler);
  cmdList.bind_texture(mTexture);

  const f32 aspectRatio {static_cast<f32>(drawContext.viewport.width()) /
                         static_cast<f32>(drawContext.viewport.height())};
  const auto projection {
    Matrix4x4f32::perspective_projection(mFov, aspectRatio, 0.1f, 250.0f)};

  cmdList.set_transform(TransformState::ViewToViewport, projection);

  const Vector3f32 lookAt {
    mCameraPos + Vector3f32 {mCameraAngleY.sin(), 0.0f, mCameraAngleY.cos()}};

  const auto view {
    Matrix4x4f32::look_at_lh(mCameraPos, lookAt, Vector3f32::up())};
  cmdList.set_transform(TransformState::WorldToView, view);
  cmdList.set_transform(TransformState::ModelToWorld, Matrix4x4f32::identity());

  cmdList.bind_vertex_buffer(mVertexBuffer, 0);
  cmdList.bind_index_buffer(mIndexBuffer);

  cmdList.draw_indexed(0, 0, NUM_CUBES * NUM_VERTICES_PER_CUBE, 0,
                       NUM_CUBES * NUM_INDICES_PER_CUBE);

  drawContext.commandLists.emplace_back(std::move(cmdList));
}

auto Buffers::on_tick(Engine& engine) -> void {
  const auto dt {static_cast<f32>(engine.delta_time())};

  if (is_key_down(Key::LeftArrow)) {
    mCameraAngleY -= Angle::degrees(45.0f * dt);
  }
  if (is_key_down(Key::RightArrow)) {
    mCameraAngleY += Angle::degrees(45.0f * dt);
  }

  while (mCameraAngleY.degrees() < -180) {
    mCameraAngleY += 360_deg;
  }

  while (mCameraAngleY.degrees() > 180) {
    mCameraAngleY -= 360_deg;
  }

  const Vector3f32 dir {mCameraAngleY.sin(), 0.0f, mCameraAngleY.cos()};

  if (is_key_down(Key::UpArrow)) {
    mCameraPos += dir * (10.0f * dt);
  }
  if (is_key_down(Key::DownArrow)) {
    mCameraPos -= dir * (10.0f * dt);
  }

  if (is_key_down(Key::NumpadSub) || is_key_down(Key::Minus)) {
    mFov -= Angle::degrees(15.f * dt);
  }
  if (is_key_down(Key::NumpadAdd) || is_key_down(Key::Plus)) {
    mFov += Angle::degrees(15.f * dt);
  }

  if (mFov.degrees() <= 0) {
    mFov = 0.1_deg;
  }
  if (mFov.degrees() >= 180) {
    mFov = 179.9_deg;
  }
}

auto Buffers::on_input(const InputEvent&) -> InputEventHandled {
  return InputEventHandled::Yes;
}

} // namespace tribase
