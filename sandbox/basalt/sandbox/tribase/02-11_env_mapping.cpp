#include <basalt/sandbox/tribase/02-11_env_mapping.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/backend/buffer.h>
#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/pipeline.h>
#include <basalt/api/gfx/backend/vertex_layout.h>
#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <basalt/api/math/matrix4.h>

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
using basalt::gfx::IndexBufferCreateInfo;
using basalt::gfx::LightData;
using basalt::gfx::PipelineCreateInfo;
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
using basalt::gfx::VertexBufferCreateInfo;
using basalt::gfx::VertexElement;
using basalt::gfx::ext::XMeshCommandEncoder;

namespace {

struct Vertex {
  Vector3f32 pos{};
  Vector3f32 tex1{};

  static constexpr auto sLayout =
    basalt::gfx::make_vertex_layout<VertexElement::Position3F32,
                                    VertexElement::TextureCoords3F32>();
};

constexpr auto ENV_TEXTURE_PATH = "data/tribase/02-11_env_mapping/EnvMap.dds"sv;
constexpr auto TEXTURE_PATH = "data/tribase/02-11_env_mapping/Texture.bmp"sv;
constexpr auto SPHERE_PATH = "data/tribase/02-11_env_mapping/Sphere.x"sv;

} // namespace

EnvMapping::EnvMapping(Engine const& engine)
  : mGfxCache{engine.gfx_context().create_resource_cache()}
  , mSampler{mGfxCache->create_sampler({TextureFilter::Bilinear,
                                        TextureFilter::Bilinear,
                                        TextureMipFilter::Linear})}
  , mEnvTexture{mGfxCache->load_texture_cube(ENV_TEXTURE_PATH)}
  , mTexture{mGfxCache->load_texture_2d(TEXTURE_PATH)}
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
      VertexBufferCreateInfo{skyBoxVertexData.size_bytes(), Vertex::sLayout},
      skyBoxVertexData);
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

    return mGfxCache->create_index_buffer(
      IndexBufferCreateInfo{skyBoxIndexData.size_bytes()}, skyBoxIndexData);
  }()} {
  mSphereMesh = [&] {
    auto const xModelData = mGfxCache->load_x_meshes(SPHERE_PATH);

    return xModelData.meshes.front();
  }();

  auto textureStages = array{TextureStage{}, TextureStage{TextureOp::Add}};

  auto fs = FixedFragmentShaderCreateInfo{};
  fs.textureStages = span{textureStages}.subspan(0, 1);

  auto pipelineDesc = PipelineCreateInfo{};
  pipelineDesc.fragmentShader = &fs;
  pipelineDesc.vertexLayout = Vertex::sLayout;
  pipelineDesc.primitiveType = PrimitiveType::TriangleList;
  pipelineDesc.cullMode = CullMode::CounterClockwise;
  pipelineDesc.dithering = true;
  mSkyBoxPipeline = mGfxCache->create_pipeline(pipelineDesc);

  constexpr auto textureCoordinateSets = array{
    TextureCoordinateSet{0, TextureCoordinateSrc::ReflectionVectorInViewSpace,
                         0, TextureCoordinateTransformMode::Count3}};

  auto vs = FixedVertexShaderCreateInfo{};
  vs.textureCoordinateSets = textureCoordinateSets;
  vs.lightingEnabled = true;
  vs.specularEnabled = true;

  pipelineDesc.vertexShader = &vs;
  pipelineDesc.depthTest = TestPassCond::IfLessEqual;
  pipelineDesc.depthWriteEnable = true;
  mSphere1Pipeline = mGfxCache->create_pipeline(pipelineDesc);

  fs.textureStages = textureStages;
  mSphere2Pipeline = mGfxCache->create_pipeline(pipelineDesc);

  textureStages[1].colorOp = TextureOp::Modulate;
  mSphere3Pipeline = mGfxCache->create_pipeline(pipelineDesc);
}

auto EnvMapping::on_update(UpdateContext& ctx) -> void {
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

  cmdList.bind_pipeline(mSphere1Pipeline);

  auto lights = array<LightData, 1>{
    DirectionalLightData{Colors::WHITE, Colors::WHITE, Colors::WHITE,
                         cameraDir + Vector3f32{0, mCameraUpDown, 0}}};
  cmdList.set_lights(lights);

  cmdList.set_material(Color::from_non_linear(0.75f, 0.75f, 0.75f),
                       Color::from_non_linear(0.25f, 0.25f, 0.25f),
                       Colors::BLACK,
                       Color::from_non_linear(0.25f, 0.25f, 0.5f), 10);

  cmdList.set_transform(TransformState::LocalToWorld,
                        Matrix4x4f32::translation(1, 0, 5));
  cmdList.set_transform(TransformState::Texture0, Matrix4x4f32::identity());
  XMeshCommandEncoder::draw_x_mesh(cmdList, mSphereMesh);

  cmdList.bind_pipeline(mSphere2Pipeline);
  cmdList.bind_sampler(1, mSampler);
  cmdList.bind_texture(1, mTexture);
  cmdList.set_transform(TransformState::LocalToWorld,
                        Matrix4x4f32::translation(-1, 0, 8));
  XMeshCommandEncoder::draw_x_mesh(cmdList, mSphereMesh);

  cmdList.bind_pipeline(mSphere3Pipeline);
  cmdList.set_transform(TransformState::LocalToWorld,
                        Matrix4x4f32::translation(-3, 0, 5));
  XMeshCommandEncoder::draw_x_mesh(cmdList, mSphereMesh);

  ctx.drawCtx.commandLists.push_back(std::move(cmdList));
}

auto EnvMapping::on_input(InputEvent const&) -> InputEventHandled {
  return InputEventHandled::Yes;
}

} // namespace tribase
