#include <basalt/sandbox/tribase/02-07_lighting.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/matrix4x4.h>
#include <basalt/api/math/vector3.h>

#include <array>
#include <cmath>
#include <utility>

namespace tribase {

using std::array;

using namespace basalt::literals;
using basalt::Angle;
using basalt::Engine;
using basalt::Matrix4x4f32;
using basalt::Vector3f32;
using basalt::gfx::Attachment;
using basalt::gfx::Attachments;
using basalt::gfx::CommandList;
using basalt::gfx::CullMode;
using basalt::gfx::FixedFragmentShaderCreateInfo;
using basalt::gfx::FixedVertexShaderCreateInfo;
using basalt::gfx::FogMode;
using basalt::gfx::LightData;
using basalt::gfx::PipelineDescriptor;
using basalt::gfx::PointLightData;
using basalt::gfx::SamplerDescriptor;
using basalt::gfx::TestPassCond;
using basalt::gfx::TextureFilter;
using basalt::gfx::TextureMipFilter;
using basalt::gfx::TextureStage;
using basalt::gfx::TransformState;
using basalt::gfx::ext::XMeshCommandEncoder;

namespace {

constexpr Color BACKGROUND {Color::from_non_linear_rgba8(0, 0, 63)};

} // namespace

Lighting::Lighting(Engine& engine)
  : mGfxCache {engine.create_gfx_resource_cache()} {
  mSphereTexture =
    mGfxCache->load_texture("data/tribase/02-07_lighting/Sphere.bmp");
  mGroundTexture =
    mGfxCache->load_texture("data/tribase/02-07_lighting/Ground.bmp");
  mSphereModel =
    mGfxCache->load_x_model("data/tribase/02-07_lighting/Sphere.x");
  mGroundModel =
    mGfxCache->load_x_model("data/tribase/02-07_lighting/Ground.x");

  FixedVertexShaderCreateInfo vs;
  vs.lightingEnabled = true;
  vs.specularEnabled = true;
  vs.fog = FogMode::Exponential;

  FixedFragmentShaderCreateInfo fs;
  array textureStages {TextureStage {}};
  fs.textureStages = textureStages;

  PipelineDescriptor pipelineDesc;
  pipelineDesc.vertexShader = &vs;
  pipelineDesc.fragmentShader = &fs;
  pipelineDesc.cullMode = CullMode::CounterClockwise;
  pipelineDesc.depthTest = TestPassCond::IfLessEqual;
  pipelineDesc.depthWriteEnable = true;
  pipelineDesc.dithering = true;
  mPipeline = mGfxCache->create_pipeline(pipelineDesc);

  pipelineDesc.fragmentShader = nullptr;
  mNoTexturePipeline = mGfxCache->create_pipeline(pipelineDesc);

  SamplerDescriptor samplerDesc;
  samplerDesc.magFilter = TextureFilter::Bilinear;
  samplerDesc.minFilter = TextureFilter::Bilinear;
  samplerDesc.mipFilter = TextureMipFilter::Linear;
  mSampler = mGfxCache->create_sampler(samplerDesc);
}

auto Lighting::on_update(UpdateContext& ctx) -> void {
  mTime += ctx.deltaTime;
  const f32 t {mTime.count()};

  const auto& drawCtx {ctx.drawCtx};
  CommandList cmdList;
  cmdList.clear_attachments(
    Attachments {Attachment::RenderTarget, Attachment::DepthBuffer}, BACKGROUND,
    1);
  cmdList.bind_pipeline(mPipeline);
  cmdList.bind_sampler(0, mSampler);
  cmdList.set_fog_parameters(BACKGROUND, 0, 0, 0.01f);

  const Vector3f32 lightPos {0, std::sin(t) * 10, 0};
  PointLightData pointLight {
    Colors::WHITE, Colors::WHITE, Colors::WHITE, lightPos, 1000, 0, 0.025f, 0};
  const array<LightData, 1> lights {pointLight};
  cmdList.set_lights(lights);
  cmdList.set_ambient_light(Color::from_non_linear(0.25f, 0, 0));

  const auto viewToClip {Matrix4x4f32::perspective_projection(
    90_deg, drawCtx.viewport.aspect_ratio(), 0.1f, 500)};
  cmdList.set_transform(TransformState::ViewToClip, viewToClip);

  const auto worldToView {Matrix4x4f32::look_at_lh(
    Vector3f32 {5, 7.5f, -15}, Vector3f32 {}, Vector3f32::up())};
  cmdList.set_transform(TransformState::WorldToView, worldToView);

  // render spheres
  const auto& sphereData {mGfxCache->get(mSphereModel)};
  cmdList.bind_texture(0, mSphereTexture);

  for (i32 i {0}; i < 10; i++) {
    const f32 perSphereFactor {static_cast<f32>(i)};

    cmdList.set_material(
      Color::from_non_linear(0.75f, 0.75f, 0.75f),
      Color::from_non_linear(0.25f, 0.25f, 0.25f), Colors::BLACK,
      Color::from_non_linear(0.25f, 0.25f, 0.25f), perSphereFactor * 5);

    const auto localToWorld {
      Matrix4x4f32::translation(10, 0, 0) *
      Matrix4x4f32::rotation_y(Angle::degrees(perSphereFactor * 36 + t * 10)) *
      Matrix4x4f32::translation(0, std::sin(perSphereFactor + t * 2), 0)};
    cmdList.set_transform(TransformState::LocalToWorld, localToWorld);

    XMeshCommandEncoder::draw_x_mesh(cmdList, sphereData.meshes[0]);
  }

  // render ground
  const auto& groundData {mGfxCache->get(mGroundModel)};
  cmdList.bind_texture(0, mGroundTexture);
  cmdList.set_material(Color::from_non_linear(0.75f, 0.75f, 0.75f),
                       Color::from_non_linear(0.25f, 0.25f, 0.25f),
                       Colors::BLACK, Colors::WHITE, 1);
  auto localToWorld {Matrix4x4f32::rotation_y(Angle::radians(t)) *
                     Matrix4x4f32::translation(0, -50, 0)};
  cmdList.set_transform(TransformState::LocalToWorld, localToWorld);
  XMeshCommandEncoder::draw_x_mesh(cmdList, groundData.meshes[0]);

  // render light source
  cmdList.bind_pipeline(mNoTexturePipeline);
  cmdList.set_material(Color::from_non_linear(0.75f, 0.75f, 0.75f),
                       Color::from_non_linear(0.25f, 0.25f, 0.25f),
                       Colors::WHITE, Colors::WHITE, 1);

  localToWorld = Matrix4x4f32::translation(pointLight.positionInWorld);
  cmdList.set_transform(TransformState::LocalToWorld, localToWorld);

  XMeshCommandEncoder::draw_x_mesh(cmdList, sphereData.meshes[0]);

  drawCtx.commandLists.emplace_back(std::move(cmdList));
}

} // namespace tribase
