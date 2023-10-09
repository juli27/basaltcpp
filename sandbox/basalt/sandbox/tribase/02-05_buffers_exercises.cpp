#include <basalt/sandbox/tribase/02-05_buffers_exercises.h>

#include <basalt/api/engine.h>
#include <basalt/api/input_events.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/math/matrix4x4.h>
#include <basalt/api/math/vector2.h>

#include <gsl/span>
#include <imgui/imgui.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <random>
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
using basalt::gfx::Pipeline;
using basalt::gfx::PipelineDescriptor;
using basalt::gfx::PrimitiveType;
using basalt::gfx::SamplerDescriptor;
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
using std::vector;

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

constexpr auto NUM_CUBES = u32{2048};
constexpr auto NUM_TRIANGLES_PER_CUBE = u32{2 * 6};
constexpr auto NUM_VERTICES_PER_CUBE = u32{8};
constexpr auto NUM_INDICES_PER_CUBE = u32{NUM_TRIANGLES_PER_CUBE * 3};

static_assert(NUM_CUBES * NUM_VERTICES_PER_CUBE <= 0xffff,
              "can't index vertices with u16");

auto fill_buffers(span<Vertex> const vb, span<u16> const ib) {
  auto randomEngine = default_random_engine{random_device{}()};
  auto rng1 = Distribution{-1.0f, 1.0f};
  auto rng2 = Distribution{0.0f, 1.0f};

  constexpr auto cubePositions = array{
    Vector3f32{-1.0f, 1.0f, -1.0f},  Vector3f32{-1.0f, 1.0f, 1.0f},
    Vector3f32{1.0f, 1.0f, 1.0f},    Vector3f32{1.0f, 1.0f, -1.0f},
    Vector3f32{-1.0f, -1.0f, -1.0f}, Vector3f32{-1.0f, -1.0f, 1.0f},
    Vector3f32{1.0f, -1.0f, 1.0f},   Vector3f32{1.0f, -1.0f, -1.0f},
  };

  for (auto i = uSize{0}; i < NUM_VERTICES_PER_CUBE; i++) {
    auto const vertexColor =
      2.0f * Color::from_non_linear(rng2(randomEngine), rng2(randomEngine),
                                    rng2(randomEngine));
    vb[i] = {
      cubePositions[i],
      vertexColor.to_argb(),
      {rng1(randomEngine), rng1(randomEngine)},
    };
  }

  constexpr auto cubeIndices =
    array<u16, NUM_INDICES_PER_CUBE>{0, 3, 7, 0, 7, 4, // front
                                     2, 1, 5, 2, 5, 6, // back
                                     1, 0, 4, 1, 4, 5, // left
                                     3, 2, 6, 3, 6, 7, // right
                                     0, 1, 2, 0, 2, 3, // top
                                     6, 5, 4, 6, 4, 7}; // bottom

  std::copy(cubeIndices.begin(), cubeIndices.end(), ib.begin());
}

} // namespace

BuffersExercises::BuffersExercises(Engine const& engine)
  : mCubes{[&] {
    auto randomEngine = default_random_engine{random_device{}()};
    auto rng1 = Distribution{-1.0f, 1.0f};
    auto rng2 = Distribution{20.0f, 250.f};
    auto rng3 = Distribution{0.1f, 5.0f};

    auto const normalizedRandomVector = [&] {
      return Vector3f32::normalize(rng1(randomEngine), rng1(randomEngine),
                                   rng1(randomEngine));
    };

    auto cubes = vector<CubeData>{};
    cubes.reserve(NUM_CUBES);
    std::generate_n(std::back_inserter(cubes), NUM_CUBES, [&] {
      auto const position = normalizedRandomVector() * rng2(randomEngine);
      auto const velocity = normalizedRandomVector() * rng3(randomEngine);

      return CubeData{position, velocity};
    });

    return cubes;
  }()}
  , mGfxCache{engine.create_gfx_resource_cache()}
  , mPipeline{[&] {
    auto fs = FixedFragmentShaderCreateInfo{};
    constexpr auto textureStages = array{TextureStage{}};
    fs.textureStages = textureStages;

    auto desc = PipelineDescriptor{};
    desc.fragmentShader = &fs;
    desc.vertexLayout = Vertex::sLayout;
    desc.primitiveType = PrimitiveType::TriangleList;
    desc.depthTest = TestPassCond::IfLessEqual;
    desc.depthWriteEnable = true;
    desc.dithering = true;

    return mGfxCache->create_pipeline(desc);
  }()}
  , mVertexBuffer{mGfxCache->create_vertex_buffer(
      {NUM_VERTICES_PER_CUBE * sizeof(Vertex), Vertex::sLayout})}
  , mIndexBuffer{mGfxCache->create_index_buffer(
      {NUM_INDICES_PER_CUBE * sizeof(u16)})}
  , mSampler{[&] {
    auto desc = SamplerDescriptor{};
    desc.magFilter = TextureFilter::Bilinear;
    desc.minFilter = TextureFilter::Bilinear;
    desc.mipFilter = TextureMipFilter::Linear;

    return mGfxCache->create_sampler(desc);
  }()}
  , mTexture{mGfxCache->load_texture("data/tribase/Texture2.bmp"sv)}
  , mFov{90_deg} {
  mGfxCache->with_mapping_of(mVertexBuffer, [&](span<byte> const vbData) {
    auto const vb = span<Vertex>{reinterpret_cast<Vertex*>(vbData.data()),
                                 vbData.size() / sizeof(Vertex)};

    mGfxCache->with_mapping_of(mIndexBuffer, [&](span<byte> const ibData) {
      auto const ib = span<u16>{reinterpret_cast<u16*>(ibData.data()),
                                ibData.size() / sizeof(u16)};
      fill_buffers(vb, ib);
    });
  });
}

auto BuffersExercises::regenerate_velocities() -> void {
  auto randomEngine = default_random_engine{random_device{}()};
  auto rng1 = Distribution{-1.0f, 1.0f};
  auto rng2 = Distribution{0.1f, 5.0f};

  auto const normalizedRandomVector = [&] {
    return Vector3f32::normalize(rng1(randomEngine), rng1(randomEngine),
                                 rng1(randomEngine));
  };

  for (auto& cube : mCubes) {
    cube.velocity = normalizedRandomVector() * rng2(randomEngine);
  }
}

auto BuffersExercises::on_update(UpdateContext& ctx) -> void {
  if (ImGui::Begin("Settings##TribaseTexturesEx")) {
    if (ImGui::RadioButton("Exercise 1", &mCurrentExercise, 0)) {
      regenerate_velocities();
    }

    ImGui::RadioButton("Exercise 2", &mCurrentExercise, 1);
  }

  ImGui::End();

  auto const dt = ctx.deltaTime.count();

  if (mCurrentExercise == 1) {
    for (auto& cube : mCubes) {
      auto const toCamera = mCameraPos - cube.position;
      cube.velocity += 0.01f * dt * toCamera;
    }
  }

  if (!is_key_down(Key::Space)) {
    for (auto& cube : mCubes) {
      cube.position += cube.velocity * dt;

      if (cube.position.length() > 250) {
        cube.velocity *= -1.0f;
      }
    }
  }

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

  auto const& drawCtx = ctx.drawCtx;
  auto const aspectRatio = drawCtx.viewport.aspect_ratio();
  cmdList.set_transform(
    TransformState::ViewToClip,
    Matrix4x4f32::perspective_projection(mFov, aspectRatio, 0.1f, 250.0f));

  auto const lookAt =
    mCameraPos + Vector3f32{mCameraAngleY.sin(), 0.0f, mCameraAngleY.cos()};
  cmdList.set_transform(
    TransformState::WorldToView,
    Matrix4x4f32::look_at_lh(mCameraPos, lookAt, Vector3f32::up()));

  for (auto const& cube : mCubes) {
    cmdList.set_transform(TransformState::LocalToWorld,
                          Matrix4x4f32::translation(cube.position));
    cmdList.bind_vertex_buffer(mVertexBuffer);
    cmdList.bind_index_buffer(mIndexBuffer);
    cmdList.draw_indexed(0, 0, NUM_VERTICES_PER_CUBE, 0, NUM_INDICES_PER_CUBE);
  }

  drawCtx.commandLists.emplace_back(std::move(cmdList));
}

auto BuffersExercises::on_input(InputEvent const&) -> InputEventHandled {
  return InputEventHandled::Yes;
}

namespace {

// TODO: exercise 3 + demo
// auto transform_vertex_positions(const span<byte> vertexData,
//                                const VertexLayout layout,
//                                const Matrix4x4f32& transform) -> void {
//}

} // namespace

} // namespace tribase
