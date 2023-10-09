#include <basalt/sandbox/tribase/02-12_bump_mapping.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <basalt/api/math/matrix4x4.h>

#include <gsl/span>

#include <algorithm>
#include <array>
#include <cstddef>
#include <string_view>
#include <utility>

namespace tribase {

using namespace std::literals;
using std::array;
using std::byte;

using gsl::span;

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
using basalt::gfx::CullMode;
using basalt::gfx::DirectionalLightData;
using basalt::gfx::FixedFragmentShaderCreateInfo;
using basalt::gfx::FixedVertexShaderCreateInfo;
using basalt::gfx::LightData;
using basalt::gfx::PipelineDescriptor;
using basalt::gfx::PrimitiveType;
using basalt::gfx::TestPassCond;
using basalt::gfx::TextureCoordinateSet;
using basalt::gfx::TextureCoordinateSrc;
using basalt::gfx::TextureCoordinateTransformMode;
using basalt::gfx::TextureFilter;
using basalt::gfx::TextureMipFilter;
using basalt::gfx::TextureOp;
using basalt::gfx::TextureStage;
using basalt::gfx::TransformState;
using basalt::gfx::VertexElement;
using basalt::gfx::ext::XMeshCommandEncoder;

namespace {

struct Vertex {
  Vector3f32 pos{};
  Vector3f32 tex0{};

  static constexpr auto sLayout = array{
    VertexElement::Position3F32,
    VertexElement::TextureCoords3F32,
  };
};

constexpr auto ENV_TEXTURE_PATH = "data/tribase/02-11_env_mapping/EnvMap.dds"sv;
constexpr auto TEXTURE_PATH = "data/tribase/02-12_bump_mapping/Texture.jpg"sv;
constexpr auto BUMP_MAP_PATH = "data/tribase/02-12_bump_mapping/BumpMap.dds"sv;
constexpr auto CUBE_PATH = "data/tribase/02-09_multi_tex/Cube.x"sv;

} // namespace

BumpMapping::BumpMapping(Engine const& engine)
  : mGfxCache{engine.gfx_context().create_resource_cache()}
  , mSampler{mGfxCache->create_sampler({TextureFilter::Bilinear,
                                        TextureFilter::Bilinear,
                                        TextureMipFilter::Linear})}
  , mEnvTexture{mGfxCache->load_cube_texture(ENV_TEXTURE_PATH)}
  , mBaseTexture{mGfxCache->load_texture(TEXTURE_PATH)}
  , mBumpMap{mGfxCache->load_texture(BUMP_MAP_PATH)}
  , mCube{mGfxCache->load_x_model(CUBE_PATH)}
  , mSkyBoxVb{[&] {
    constexpr auto skyBoxVertices =
      array{Vertex{{-1.0f, 1.0f, 1.0f}, {-1.0f, 1.0f, 1.0f}},
            Vertex{{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
            Vertex{{1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, -1.0f}},
            Vertex{{-1.0f, 1.0f, -1.0f}, {-1.0f, 1.0f, -1.0f}},
            Vertex{{-1.0f, -1.0f, 1.0f}, {-1.0f, -1.0f, 1.0f}},
            Vertex{{1.0f, -1.0f, 1.0f}, {1.0f, -1.0f, 1.0f}},
            Vertex{{1.0f, -1.0f, -1.0f}, {1.0f, -1.0f, -1.0f}},
            Vertex{{-1.0f, -1.0f, -1.0f}, {-1.0f, -1.0f, -1.0f}}};
    auto const skyBoxVertexData = as_bytes(span{skyBoxVertices});

    return mGfxCache->create_vertex_buffer(
      {skyBoxVertexData.size_bytes(), Vertex::sLayout}, skyBoxVertexData);
  }()}
  , mSkyBoxIb{[&] {
    constexpr auto skyBoxIndices = array<u16, 36>{
      7, 3, 0, 4, 7, 0, // front
      5, 1, 2, 6, 5, 2, // back
      4, 0, 1, 5, 4, 1, // left
      6, 2, 3, 7, 6, 3, // right
      2, 1, 0, 3, 2, 0, // top
      4, 5, 6, 7, 4, 6, // bottom
    };
    auto const skyBoxIndexData = as_bytes(span{skyBoxIndices});

    return mGfxCache->create_index_buffer({skyBoxIndexData.size_bytes()},
                                          skyBoxIndexData);
  }()} {
  auto textureStages =
    array{TextureStage{}, TextureStage{TextureOp::BumpEnvMap},
          TextureStage{TextureOp::Add}};

  auto fs = FixedFragmentShaderCreateInfo{};
  fs.textureStages = span{textureStages}.subspan(0, 1);

  auto pipelineDesc = PipelineDescriptor{};
  pipelineDesc.fragmentShader = &fs;
  pipelineDesc.vertexLayout = Vertex::sLayout;
  pipelineDesc.primitiveType = PrimitiveType::TriangleList;
  pipelineDesc.cullMode = CullMode::CounterClockwise;
  pipelineDesc.dithering = true;
  mSkyBoxPipeline = mGfxCache->create_pipeline(pipelineDesc);

  auto textureCoordinateSets = array{
    TextureCoordinateSet{2, TextureCoordinateSrc::ReflectionVectorInViewSpace,
                         0, TextureCoordinateTransformMode::Count3}};

  auto vs = FixedVertexShaderCreateInfo{};
  vs.textureCoordinateSets = textureCoordinateSets;
  vs.lightingEnabled = true;

  fs.textureStages = textureStages;

  pipelineDesc.vertexShader = &vs;
  pipelineDesc.depthTest = TestPassCond::IfLessEqual;
  pipelineDesc.depthWriteEnable = true;
  mCube1Pipeline = mGfxCache->create_pipeline(pipelineDesc);

  std::get<1>(textureStages) = std::get<2>(textureStages);
  std::get<0>(textureCoordinateSets).stageIndex = 1;
  fs.textureStages = span{textureStages}.subspan(0, 2);
  mCube2Pipeline = mGfxCache->create_pipeline(pipelineDesc);
}

auto BumpMapping::on_update(UpdateContext& ctx) -> void {
  mTime += ctx.deltaTime;
  auto const t = mTime.count();
  auto const dt = ctx.deltaTime.count();

  if (is_key_down(Key::LeftArrow)) {
    mCameraAngleY -= Angle::degrees(45.0f * dt);
  }
  if (is_key_down(Key::RightArrow)) {
    mCameraAngleY += Angle::degrees(45.0f * dt);
  }

  auto const cameraDir =
    Vector3f32{mCameraAngleY.sin(), 0, mCameraAngleY.cos()};
  if (is_key_down(Key::UpArrow)) {
    mCameraPos += cameraDir * 5.0f * dt;
  }
  if (is_key_down(Key::DownArrow)) {
    mCameraPos -= cameraDir * 5.0f * dt;
  }

  if (is_key_down(Key::NumpadSub)) {
    mCameraUpDown += dt;
  }
  if (is_key_down(Key::NumpadAdd)) {
    mCameraUpDown -= dt;
  }

  mCameraUpDown = std::clamp(mCameraUpDown, -2.5f, 2.5f);

  auto cmdList = CommandList{};
  cmdList.clear_attachments(Attachments{Attachment::DepthBuffer}, {}, 1.0f);
  cmdList.bind_pipeline(mSkyBoxPipeline);
  cmdList.bind_sampler(0, mSampler);
  cmdList.bind_texture(0, mEnvTexture);

  cmdList.set_transform(
    TransformState::ViewToClip,
    Matrix4x4f32::perspective_projection(
      70_deg, ctx.drawCtx.viewport.aspect_ratio(), 0.1f, 100.0f));
  cmdList.set_transform(
    TransformState::WorldToView,
    Matrix4x4f32::look_at_lh(
      mCameraPos, mCameraPos + cameraDir + Vector3f32{0, mCameraUpDown, 0},
      Vector3f32::up()));
  cmdList.set_transform(TransformState::LocalToWorld,
                        Matrix4x4f32::translation(mCameraPos));

  cmdList.bind_vertex_buffer(mSkyBoxVb);
  cmdList.bind_index_buffer(mSkyBoxIb);
  cmdList.draw_indexed(0, 0, 8, 0, 36);

  auto const& cubeData = mGfxCache->get(mCube);
  cmdList.bind_pipeline(mCube1Pipeline);
  cmdList.bind_texture(0, mBaseTexture);
  cmdList.bind_sampler(1, mSampler);
  cmdList.bind_texture(1, mBumpMap);
  cmdList.bind_sampler(2, mSampler);
  cmdList.bind_texture(2, mEnvTexture);

  auto lights = array<LightData, 1>{
    DirectionalLightData{Colors::WHITE, {}, Colors::WHITE, Vector3f32{1}}};
  cmdList.set_lights(lights);
  cmdList.set_material(Color::from_non_linear(0.75f, 0.75f, 0.75f),
                       Color::from_non_linear(0.25f, 0.25f, 0.25f));
  cmdList.set_transform(TransformState::LocalToWorld,
                        Matrix4x4f32::rotation_y(Angle::radians(0.25f * t)) *
                          Matrix4x4f32::translation(1, 0, 2.5f));
  cmdList.set_transform(TransformState::Texture2, Matrix4x4f32::identity());
  XMeshCommandEncoder::draw_x_mesh(cmdList, cubeData.meshes[0]);

  cmdList.bind_pipeline(mCube2Pipeline);
  cmdList.bind_texture(1, mEnvTexture);
  cmdList.set_transform(TransformState::LocalToWorld,
                        Matrix4x4f32::rotation_y(Angle::radians(0.25f * t)) *
                          Matrix4x4f32::translation(-1, 0, 2.5f));
  cmdList.set_transform(TransformState::Texture1, Matrix4x4f32::identity());
  XMeshCommandEncoder::draw_x_mesh(cmdList, cubeData.meshes[0]);

  ctx.drawCtx.commandLists.push_back(std::move(cmdList));
}

auto BumpMapping::on_input(InputEvent const&) -> InputEventHandled {
  return InputEventHandled::Yes;
}

} // namespace tribase
