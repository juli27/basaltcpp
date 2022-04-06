#include <basalt/sandbox/tribase/02-05_buffers.h>

#include <basalt/api/engine.h>
#include <basalt/api/input_events.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/math/matrix4x4.h>
#include <basalt/api/math/vector2.h>

#include <gsl/span>

#include <array>
#include <cstddef>
#include <random>
#include <string_view>
#include <utility>

using namespace basalt::literals;

using basalt::Angle;
using basalt::Engine;
using basalt::InputEvent;
using basalt::InputEventHandled;
using basalt::Key;
using basalt::Matrix4x4f32;
using basalt::Vector2f32;
using basalt::Vector3f32;
using basalt::gfx::Attachment;
using basalt::gfx::Attachments;
using basalt::gfx::CommandList;
using basalt::gfx::FixedFragmentShaderCreateInfo;
using basalt::gfx::PipelineCreateInfo;
using basalt::gfx::PipelineHandle;
using basalt::gfx::PrimitiveType;
using basalt::gfx::TestPassCond;
using basalt::gfx::TextureFilter;
using basalt::gfx::TextureMipFilter;
using basalt::gfx::TextureStage;
using basalt::gfx::TransformState;
using basalt::gfx::VertexElement;

using gsl::span;

using namespace std::literals;
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
  ColorEncoding::A8R8G8B8 diffuse{};
  Vector2f32 uv{};

  static constexpr auto sLayout = array{
    VertexElement::Position3F32,
    VertexElement::ColorDiffuse1U32A8R8G8B8,
    VertexElement::TextureCoords2F32,
  };
};

constexpr auto TEXTURE_FILE_PATH = "data/tribase/Texture2.bmp"sv;

constexpr auto NUM_CUBES = u32{2048};
constexpr auto NUM_TRIANGLES_PER_CUBE = u32{2 * 6};
constexpr auto NUM_VERTICES_PER_CUBE = u32{8};
constexpr auto NUM_INDICES_PER_CUBE = u32{NUM_TRIANGLES_PER_CUBE * 3};

static_assert(NUM_CUBES * NUM_VERTICES_PER_CUBE <= 0xffff,
              "can't index vertices with u16");

auto fill_buffers(span<Vertex> const vb, span<u16> const ib) {
  auto randomEngine = default_random_engine{random_device{}()};
  auto rng1 = Distribution{-1.0f, 1.0f};
  auto rng2 = Distribution{20.0f, 250.f};
  auto rng3 = Distribution{0.0f, 1.0f};

  auto const normalizedRandomVector = [&] {
    return Vector3f32::normalize(rng1(randomEngine), rng1(randomEngine),
                                 rng1(randomEngine));
  };

  constexpr auto cubeIndices =
    array<u16, NUM_INDICES_PER_CUBE>{0, 3, 7, 0, 7, 4, // front
                                     2, 1, 5, 2, 5, 6, // back
                                     1, 0, 4, 1, 4, 5, // left
                                     3, 2, 6, 3, 6, 7, // right
                                     0, 1, 2, 0, 2, 3, // top
                                     6, 5, 4, 6, 4, 7}; // bottom

  for (auto iCube = u32{0}; iCube < NUM_CUBES; iCube++) {
    auto const pos = normalizedRandomVector() * rng2(randomEngine);
    auto const startVertex = static_cast<u16>(NUM_VERTICES_PER_CUBE * iCube);

    vb[startVertex + 0].pos = pos + Vector3f32{-1.0f, 1.0f, -1.0f};
    vb[startVertex + 1].pos = pos + Vector3f32{-1.0f, 1.0f, 1.0f};
    vb[startVertex + 2].pos = pos + Vector3f32{1.0f, 1.0f, 1.0f};
    vb[startVertex + 3].pos = pos + Vector3f32{1.0f, 1.0f, -1.0f};
    vb[startVertex + 4].pos = pos + Vector3f32{-1.0f, -1.0f, -1.0f};
    vb[startVertex + 5].pos = pos + Vector3f32{-1.0f, -1.0f, 1.0f};
    vb[startVertex + 6].pos = pos + Vector3f32{1.0f, -1.0f, 1.0f};
    vb[startVertex + 7].pos = pos + Vector3f32{1.0f, -1.0f, -1.0f};

    for (auto& vertex : vb.subspan(startVertex, NUM_VERTICES_PER_CUBE)) {
      auto const vertexColor =
        Color::from_non_linear(rng3(randomEngine), rng3(randomEngine),
                               rng3(randomEngine)) *
        2.0f;
      vertex.diffuse = vertexColor.to_argb();
      vertex.uv = {rng1(randomEngine), rng1(randomEngine)};
    }

    auto const startIndex = NUM_INDICES_PER_CUBE * iCube;

    for (auto iIndex = u32{0}; iIndex < NUM_INDICES_PER_CUBE; iIndex++) {
      // add startIndex as offset to every index
      ib[startIndex + iIndex] = cubeIndices[iIndex] + startVertex;
    }
  }
}

} // namespace

Buffers::Buffers(Engine const& engine)
  : mGfxCache{engine.create_gfx_resource_cache()}
  , mSampler{mGfxCache->create_sampler({TextureFilter::Bilinear,
                                        TextureFilter::Bilinear,
                                        TextureMipFilter::Linear})}
  , mTexture{mGfxCache->load_texture_2d(TEXTURE_FILE_PATH)}
  , mVertexBuffer{mGfxCache->create_vertex_buffer(
      {NUM_VERTICES_PER_CUBE * sizeof(Vertex) * NUM_CUBES, Vertex::sLayout})}
  , mIndexBuffer{mGfxCache->create_index_buffer(
      {NUM_INDICES_PER_CUBE * sizeof(u16) * NUM_CUBES})}
  , mPipeline{[&] {
    auto fs = FixedFragmentShaderCreateInfo{};
    constexpr auto textureStages = array{TextureStage{}};
    fs.textureStages = textureStages;

    auto desc = PipelineCreateInfo{};
    desc.fragmentShader = &fs;
    desc.vertexLayout = Vertex::sLayout;
    desc.primitiveType = PrimitiveType::TriangleList;
    desc.depthTest = TestPassCond::IfLessEqual;
    desc.depthWriteEnable = true;
    desc.dithering = true;

    return mGfxCache->create_pipeline(desc);
  }()}
  , mFov{90_deg} {
  auto const& gfxCtx = engine.gfx_context();
  gfxCtx.with_mapping_of(mVertexBuffer, [&](span<byte> const vbData) {
    auto const vb = span{reinterpret_cast<Vertex*>(vbData.data()),
                         vbData.size() / sizeof(Vertex)};

    gfxCtx.with_mapping_of(mIndexBuffer, [&](span<byte> const ibData) {
      auto const ib = span{reinterpret_cast<u16*>(ibData.data()),
                           ibData.size() / sizeof(u16)};
      fill_buffers(vb, ib);
    });
  });
}

auto Buffers::on_update(UpdateContext& ctx) -> void {
  auto const dt = ctx.deltaTime.count();

  if (is_key_down(Key::LeftArrow)) {
    mCameraAngleY -= Angle::degrees(45.0f * dt);
  }
  if (is_key_down(Key::RightArrow)) {
    mCameraAngleY += Angle::degrees(45.0f * dt);
  }

  auto const dir = Vector3f32{mCameraAngleY.sin(), 0.0f, mCameraAngleY.cos()};

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

  auto cmdList = CommandList{};
  cmdList.clear_attachments(
    Attachments{Attachment::RenderTarget, Attachment::DepthBuffer},
    Colors::BLACK, 1.0f);
  cmdList.bind_pipeline(mPipeline);
  cmdList.bind_sampler(0, mSampler);
  cmdList.bind_texture(0, mTexture);

  auto const drawCtx = ctx.drawCtx;
  auto const aspectRatio = drawCtx.viewport.aspect_ratio();
  cmdList.set_transform(
    TransformState::ViewToClip,
    Matrix4x4f32::perspective_projection(mFov, aspectRatio, 0.1f, 250.0f));

  auto const lookAt =
    mCameraPos + Vector3f32{mCameraAngleY.sin(), 0.0f, mCameraAngleY.cos()};
  cmdList.set_transform(
    TransformState::WorldToView,
    Matrix4x4f32::look_at_lh(mCameraPos, lookAt, Vector3f32::up()));

  cmdList.set_transform(TransformState::LocalToWorld, Matrix4x4f32::identity());
  cmdList.bind_vertex_buffer(mVertexBuffer);
  cmdList.bind_index_buffer(mIndexBuffer);
  cmdList.draw_indexed(0, 0, NUM_CUBES * NUM_VERTICES_PER_CUBE, 0,
                       NUM_CUBES * NUM_INDICES_PER_CUBE);

  drawCtx.commandLists.emplace_back(std::move(cmdList));
}

auto Buffers::on_input(InputEvent const&) -> InputEventHandled {
  return InputEventHandled::Yes;
}

} // namespace tribase
