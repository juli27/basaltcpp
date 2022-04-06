#include <basalt/sandbox/tribase/02-09_multi_texturing.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/matrix4x4.h>
#include <basalt/api/math/vector3.h>

#include <gsl/span>

#include <array>
#include <cmath>
#include <string_view>
#include <utility>

namespace tribase {

using namespace std::literals;
using std::array;

using gsl::span;

using namespace basalt::literals;
using basalt::Angle;
using basalt::Engine;
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
using basalt::gfx::PipelineCreateInfo;
using basalt::gfx::TestPassCond;
using basalt::gfx::TextureCoordinateSet;
using basalt::gfx::TextureCoordinateTransformMode;
using basalt::gfx::TextureFilter;
using basalt::gfx::TextureMipFilter;
using basalt::gfx::TextureOp;
using basalt::gfx::TextureStage;
using basalt::gfx::TransformState;
using basalt::gfx::ext::XMeshCommandEncoder;

namespace {

constexpr auto TEXTURE1_FILE_PATH =
  "data/tribase/02-09_multi_tex/Texture1.jpg"sv;
constexpr auto TEXTURE2_FILE_PATH =
  "data/tribase/02-09_multi_tex/Texture2.jpg"sv;
constexpr auto CUBE_MODEL_FILE_PATH = "data/tribase/02-09_multi_tex/Cube.x"sv;

constexpr auto to_matrix3x3(Matrix4x4f32 const& m) -> Matrix4x4f32 {
  // clang-format off
  return Matrix4x4f32{
    m.m11, m.m12, m.m14, 0,
    m.m21, m.m22, m.m24, 0,
    m.m41, m.m42, m.m44, 0,
    0,     0,     0,     1
  };
  // clang-format on
}

} // namespace

MultiTexturing::MultiTexturing(Engine const& engine)
  : mGfxCache{engine.create_gfx_resource_cache()}
  , mSampler{mGfxCache->create_sampler({TextureFilter::Bilinear,
                                        TextureFilter::Bilinear,
                                        TextureMipFilter::Linear})}
  , mTexture0{mGfxCache->load_texture_2d(TEXTURE1_FILE_PATH)}
  , mTexture1{mGfxCache->load_texture_2d(TEXTURE2_FILE_PATH)} {
  auto const& gfxCtx = engine.gfx_context();

  mCubeMesh = [&] {
    auto const xModelHandle = mGfxCache->load_x_model({CUBE_MODEL_FILE_PATH});
    auto const& xModelData = gfxCtx.get(xModelHandle);

    return xModelData.meshes.front();
  }();

  auto vs = FixedVertexShaderCreateInfo{};
  auto textureCoordinateSets = array{TextureCoordinateSet{1}};
  std::get<0>(textureCoordinateSets).transformMode =
    TextureCoordinateTransformMode::Count2;
  vs.textureCoordinateSets = textureCoordinateSets;
  vs.lightingEnabled = true;

  auto fs = FixedFragmentShaderCreateInfo{};
  auto textureStages = array{TextureStage{}, TextureStage{}};
  fs.textureStages = textureStages;

  auto pipelineDesc = PipelineCreateInfo{};
  pipelineDesc.vertexShader = &vs;
  pipelineDesc.fragmentShader = &fs;
  pipelineDesc.cullMode = CullMode::CounterClockwise;
  pipelineDesc.depthTest = TestPassCond::IfLessEqual;
  pipelineDesc.depthWriteEnable = true;
  pipelineDesc.dithering = true;
  mPipelines[0] = mGfxCache->create_pipeline(pipelineDesc);

  textureStages[1].colorOp = TextureOp::Modulate2X;
  mPipelines[1] = mGfxCache->create_pipeline(pipelineDesc);

  textureStages[1].colorOp = TextureOp::Modulate4X;
  mPipelines[2] = mGfxCache->create_pipeline(pipelineDesc);

  textureStages[1].colorOp = TextureOp::Add;
  mPipelines[3] = mGfxCache->create_pipeline(pipelineDesc);

  textureStages[1].colorOp = TextureOp::Subtract;
  mPipelines[4] = mGfxCache->create_pipeline(pipelineDesc);

  std::swap(textureStages[1].colorArg1, textureStages[1].colorArg2);
  mPipelines[5] = mGfxCache->create_pipeline(pipelineDesc);

  textureStages[1].colorOp = TextureOp::AddSigned;
  mPipelines[6] = mGfxCache->create_pipeline(pipelineDesc);

  textureStages[1].colorOp = TextureOp::AddSigned2X;
  mPipelines[7] = mGfxCache->create_pipeline(pipelineDesc);

  textureStages[1].colorOp = TextureOp::AddSmooth;
  mPipelines[8] = mGfxCache->create_pipeline(pipelineDesc);

  textureStages[1].colorOp = TextureOp::BlendFactorAlpha;
  mPipelines[9] = mGfxCache->create_pipeline(pipelineDesc);

  textureStages[1].colorOp = TextureOp::DotProduct3;
  mPipelines[10] = mGfxCache->create_pipeline(pipelineDesc);

  // disable second stage
  fs.textureStages = span{textureStages}.subspan(0, 1);
  mPipelines[11] = mGfxCache->create_pipeline(pipelineDesc);
}

auto MultiTexturing::on_update(UpdateContext& ctx) -> void {
  mTime += ctx.deltaTime;
  auto const t = mTime.count();

  auto const& drawCtx = ctx.drawCtx;
  auto cmdList = CommandList{};
  cmdList.clear_attachments(
    Attachments{Attachment::RenderTarget, Attachment::DepthBuffer},
    Color::from_non_linear_rgba8(0, 63, 0), 1.0f);

  auto const lightDir = Vector3f32{std::sin(t), std::cos(t), 1.0f};
  auto lightData = array<LightData, 1>{
    DirectionalLightData{Colors::WHITE, {}, Colors::WHITE, lightDir}};
  cmdList.set_lights(lightData);

  cmdList.set_transform(TransformState::ViewToClip,
                        Matrix4x4f32::perspective_projection(
                          90_deg, drawCtx.viewport.aspect_ratio(), 0.1f, 500));
  cmdList.set_transform(TransformState::WorldToView, Matrix4x4f32::identity());

  cmdList.bind_sampler(0, mSampler);
  cmdList.bind_texture(0, mTexture0);
  cmdList.bind_sampler(1, mSampler);
  cmdList.bind_texture(1, mTexture1);

  cmdList.set_transform(
    TransformState::Texture1,
    to_matrix3x3(Matrix4x4f32::translation(0.5f * t, 0, 0)));

  cmdList.set_material(Color::from_non_linear(0.75f, 0.75f, 0.75f),
                       Color::from_non_linear(0.25f, 0.25f, 0.25f));

  cmdList.set_texture_factor(Color::from_non_linear_rgba8(
    0, 0, 0, static_cast<u8>(127.0f + 127.0f * std::sin(t))));

  for (auto i = uSize{0}; i < 12; ++i) {
    cmdList.bind_pipeline(mPipelines[i]);

    auto const cubePos =
      Vector3f32{7.0f * (static_cast<f32>(i % 3) - 1.0f),
                 4.5f * (-static_cast<f32>(i / 3) + 1.5f), 10};

    cmdList.set_transform(
      TransformState::LocalToWorld,
      Matrix4x4f32::scaling(2.5f) *
        Matrix4x4f32::rotation_x(Angle::radians(t)) *
        Matrix4x4f32::rotation_y(Angle::radians(0.75f * t)) *
        Matrix4x4f32::rotation_z(Angle::radians(0.5f * t)) *
        Matrix4x4f32::translation(cubePos));

    XMeshCommandEncoder::draw_x_mesh(cmdList, mCubeMesh);
  }

  drawCtx.commandLists.emplace_back(std::move(cmdList));
}

} // namespace tribase
