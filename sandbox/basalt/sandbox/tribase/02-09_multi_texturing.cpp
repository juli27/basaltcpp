#include <basalt/sandbox/tribase/02-09_multi_texturing.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/matrix4x4.h>
#include <basalt/api/math/vector3.h>

#include <gsl/span>

#include <array>
#include <cmath>
#include <utility>

namespace tribase {

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
using basalt::gfx::LightData;
using basalt::gfx::PipelineDescriptor;
using basalt::gfx::TestPassCond;
using basalt::gfx::TextureBlendingStage;
using basalt::gfx::TextureCoordinateTransformMode;
using basalt::gfx::TextureFilter;
using basalt::gfx::TextureMipFilter;
using basalt::gfx::TextureOp;
using basalt::gfx::TransformState;
using basalt::gfx::ext::XMeshCommandEncoder;

namespace {

constexpr auto to_matrix3x3(const Matrix4x4f32& m) -> Matrix4x4f32 {
  // clang-format off
  return Matrix4x4f32 {
    m.m11, m.m12, m.m14, 0,
    m.m21, m.m22, m.m24, 0,
    m.m41, m.m42, m.m44, 0,
    0,     0,     0,     1
  };
  // clang-format on
}

} // namespace

MultiTexturing::MultiTexturing(const Engine& engine)
  : mGfxCache {engine.create_gfx_resource_cache()} {
  PipelineDescriptor pipelineDesc;
  array textureStages {TextureBlendingStage {}, TextureBlendingStage {}};
  textureStages[1].coordinateTransformMode =
    TextureCoordinateTransformMode::Count2;
  pipelineDesc.textureStages = textureStages;
  pipelineDesc.lightingEnabled = true;
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
  pipelineDesc.textureStages = span {textureStages}.subspan(0, 1);
  mPipelines[11] = mGfxCache->create_pipeline(pipelineDesc);

  mSampler =
    mGfxCache->create_sampler({TextureFilter::Bilinear, TextureFilter::Bilinear,
                               TextureMipFilter::Linear});

  mTexture0 =
    mGfxCache->load_texture("data/tribase/02-09_multi_tex/Texture1.jpg");
  mTexture1 =
    mGfxCache->load_texture("data/tribase/02-09_multi_tex/Texture2.jpg");
  mCube = mGfxCache->load_x_model("data/tribase/02-09_multi_tex/Cube.x");
}

auto MultiTexturing::on_update(UpdateContext& ctx) -> void {
  mTime += ctx.deltaTime;
  const f32 t {mTime.count()};

  const auto& drawCtx {ctx.drawCtx};
  CommandList cmdList;
  cmdList.clear_attachments(
    Attachments {Attachment::RenderTarget, Attachment::DepthBuffer},
    Color::from_non_linear_rgba8(0, 63, 0), 1.0f);

  const Vector3f32 lightDir {std::sin(t), std::cos(t), 1.0f};
  array<LightData, 1> lightData {
    DirectionalLightData {Colors::WHITE, {}, Colors::WHITE, lightDir}};
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

  const auto& cubeData {mGfxCache->get(mCube)};

  for (i32 i {0}; i < 12; ++i) {
    cmdList.bind_pipeline(mPipelines[i]);

    const Vector3f32 cubePos {7.0f * (static_cast<f32>(i % 3) - 1.0f),
                              4.5f * (-static_cast<f32>(i / 3) + 1.5f), 10};

    cmdList.set_transform(
      TransformState::LocalToWorld,
      Matrix4x4f32::scaling(2.5f) *
        Matrix4x4f32::rotation_x(Angle::radians(t)) *
        Matrix4x4f32::rotation_y(Angle::radians(0.75f * t)) *
        Matrix4x4f32::rotation_z(Angle::radians(0.5f * t)) *
        Matrix4x4f32::translation(cubePos));

    XMeshCommandEncoder::draw_x_mesh(cmdList, cubeData.meshes[0]);
  }

  drawCtx.commandLists.emplace_back(std::move(cmdList));
}

} // namespace tribase
