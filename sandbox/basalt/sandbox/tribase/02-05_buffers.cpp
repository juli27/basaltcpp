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
using basalt::gfx::TestPassCond;
using basalt::gfx::TextureBlendingStage;
using basalt::gfx::TextureFilter;
using basalt::gfx::TextureMipFilter;
using basalt::gfx::TransformState;
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
  ColorEncoding::A8R8G8B8 diffuse {};
  array<f32, 2> uv {};

  static constexpr array sLayout {VertexElement::Position3F32,
                                  VertexElement::ColorDiffuse1U32A8R8G8B8,
                                  VertexElement::TextureCoords2F32};
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
    return Vector3f32::normalize(rng1(randomEngine), rng1(randomEngine),
                                 rng1(randomEngine));
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

      vertex.uv = {rng1(randomEngine), rng1(randomEngine)};
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
  : mGfxCache {engine.create_gfx_resource_cache()}
  , mTexture {mGfxCache->load_texture("data/tribase/Texture2.bmp")}
  , mFov {90_deg} {
  array textureStages {TextureBlendingStage {}};
  PipelineDescriptor pipelineDesc;
  pipelineDesc.vertexInputState = Vertex::sLayout;
  pipelineDesc.textureStages = span {textureStages};
  pipelineDesc.primitiveType = PrimitiveType::TriangleList;
  pipelineDesc.depthTest = TestPassCond::IfLessEqual;
  pipelineDesc.depthWriteEnable = true;
  pipelineDesc.dithering = true;
  mPipeline = mGfxCache->create_pipeline(pipelineDesc);

  mVertexBuffer = mGfxCache->create_vertex_buffer(
    {NUM_VERTICES_PER_CUBE * sizeof(Vertex) * NUM_CUBES, Vertex::sLayout});

  IndexBufferDescriptor ibDesc;
  ibDesc.sizeInBytes = NUM_INDICES_PER_CUBE * sizeof(u16) * NUM_CUBES;
  mIndexBuffer = mGfxCache->create_index_buffer(ibDesc);

  mGfxCache->with_mapping_of(mVertexBuffer, [this](const span<byte> vbData) {
    const span<Vertex> vb {reinterpret_cast<Vertex*>(vbData.data()),
                           vbData.size() / sizeof(Vertex)};

    mGfxCache->with_mapping_of(mIndexBuffer, [vb](const span<byte> ibData) {
      const span<u16> ib {reinterpret_cast<u16*>(ibData.data()),
                          ibData.size() / sizeof(u16)};
      fill_buffers(vb, ib);
    });
  });

  SamplerDescriptor samplerDesc;
  samplerDesc.magFilter = TextureFilter::Bilinear;
  samplerDesc.minFilter = TextureFilter::Bilinear;
  samplerDesc.mipFilter = TextureMipFilter::Linear;
  mSampler = mGfxCache->create_sampler(samplerDesc);
}

auto Buffers::on_update(UpdateContext& ctx) -> void {
  const f32 dt {ctx.deltaTime.count()};

  if (is_key_down(Key::LeftArrow)) {
    mCameraAngleY -= Angle::degrees(45.0f * dt);
  }
  if (is_key_down(Key::RightArrow)) {
    mCameraAngleY += Angle::degrees(45.0f * dt);
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
  if (mFov.degrees() >= 179) {
    mFov = 179_deg;
  }

  CommandList cmdList;
  cmdList.clear_attachments(
    Attachments {Attachment::RenderTarget, Attachment::DepthBuffer},
    Colors::BLACK, 1.0f);
  cmdList.bind_pipeline(mPipeline);
  cmdList.bind_sampler(0, mSampler);
  cmdList.bind_texture(0, mTexture);
  cmdList.bind_vertex_buffer(mVertexBuffer);
  cmdList.bind_index_buffer(mIndexBuffer);

  const DrawContext drawCtx {ctx.drawCtx};
  const f32 aspectRatio {drawCtx.viewport.aspect_ratio()};
  const auto viewToClip {
    Matrix4x4f32::perspective_projection(mFov, aspectRatio, 0.1f, 250.0f)};
  cmdList.set_transform(TransformState::ViewToClip, viewToClip);

  const Vector3f32 lookAt {
    mCameraPos + Vector3f32 {mCameraAngleY.sin(), 0.0f, mCameraAngleY.cos()}};
  const auto worldToView {
    Matrix4x4f32::look_at_lh(mCameraPos, lookAt, Vector3f32::up())};
  cmdList.set_transform(TransformState::WorldToView, worldToView);
  cmdList.set_transform(TransformState::LocalToWorld, Matrix4x4f32::identity());

  cmdList.draw_indexed(0, 0, NUM_CUBES * NUM_VERTICES_PER_CUBE, 0,
                       NUM_CUBES * NUM_INDICES_PER_CUBE);

  drawCtx.commandLists.emplace_back(std::move(cmdList));
}

auto Buffers::on_input(const InputEvent&) -> InputEventHandled {
  return InputEventHandled::Yes;
}

} // namespace tribase
