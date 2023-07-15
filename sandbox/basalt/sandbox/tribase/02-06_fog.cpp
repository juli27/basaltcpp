#include <basalt/sandbox/tribase/02-06_fog.h>

#include <basalt/api/debug_ui.h>
#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/matrix4x4.h>
#include <basalt/api/math/vector3.h>

#include <fmt/format.h>
#include <gsl/span>
#include <imgui/imgui.h>

#include <array>
#include <string>
#include <utility>

using namespace basalt::literals;

using basalt::DebugUi;
using basalt::Engine;
using basalt::Matrix4x4f32;
using basalt::Vector3f32;
using basalt::gfx::Attachment;
using basalt::gfx::Attachments;
using basalt::gfx::CommandList;
using basalt::gfx::CullMode;
using basalt::gfx::FogMode;
using basalt::gfx::FogType;
using basalt::gfx::PipelineDescriptor;
using basalt::gfx::PrimitiveType;
using basalt::gfx::SamplerDescriptor;
using basalt::gfx::TestPassCond;
using basalt::gfx::TextureBlendingStage;
using basalt::gfx::TextureFilter;
using basalt::gfx::TextureMipFilter;
using basalt::gfx::TextureOp;
using basalt::gfx::TextureStageArgument;
using basalt::gfx::TransformState;
using basalt::gfx::VertexElement;

using gsl::span;

using std::array;
using std::string;

namespace tribase {

namespace {

struct Vertex final {
  Vector3f32 pos;
  ColorEncoding::A8R8G8B8 diffuse {};
  array<f32, 2> uv {};

  static constexpr array sLayout {VertexElement::Position3F32,
                                  VertexElement::ColorDiffuse1U32A8R8G8B8,
                                  VertexElement::TextureCoords2F32};
};

} // namespace

Fog::Fog(Engine& engine) : mGfxCache {engine.create_gfx_resource_cache()} {
  update_pipeline();

  array vertices {Vertex {{-1.0f, -1.0f, 0.0f},
                          ColorEncoding::pack_a8r8g8b8_u32(191, 255, 255),
                          {0.0f, 1.0f}},
                  Vertex {{-1.0f, 1.0f, 0.0f},
                          ColorEncoding::pack_a8r8g8b8_u32(255, 191, 255),
                          {0.0f, 0.0f}},
                  Vertex {{1.0f, -1.0f, 0.0f},
                          ColorEncoding::pack_a8r8g8b8_u32(255, 255, 191),
                          {1.0f, 1.0f}},
                  Vertex {{1.0f, 1.0f, 0.0f},
                          ColorEncoding::pack_a8r8g8b8_u32(255, 255, 255),
                          {1.0f, 0.0f}}};
  const span vertexData {as_bytes(span {vertices})};
  mVertexBuffer = mGfxCache->create_vertex_buffer(
    {vertexData.size_bytes(), Vertex::sLayout}, vertexData);

  SamplerDescriptor samplerDesc;
  samplerDesc.magFilter = TextureFilter::Bilinear;
  samplerDesc.minFilter = TextureFilter::Bilinear;
  samplerDesc.mipFilter = TextureMipFilter::Linear;
  mSampler = mGfxCache->create_sampler(samplerDesc);

  for (i32 i {0}; i < sNumTextures; i++) {
    string fileName {
      fmt::format("data/tribase/02-06_fog/Texture{}.bmp", i + 1)};
    mTextures[i] = mGfxCache->load_texture(fileName);
  }
}

Fog::~Fog() noexcept {
  for (const Texture texId : mTextures) {
    mGfxCache->destroy(texId);
  }
  mGfxCache->destroy(mSampler);
  mGfxCache->destroy(mVertexBuffer);
  mGfxCache->destroy(mPipeline);
}

auto Fog::update_pipeline() -> void {
  mGfxCache->destroy(mPipeline);

  TextureBlendingStage textureStage;
  textureStage.arg1 = TextureStageArgument::SampledTexture;
  textureStage.arg2 = TextureStageArgument::Diffuse;
  textureStage.colorOp = TextureOp::Modulate;
  textureStage.alphaOp = TextureOp::SelectArg1;

  PipelineDescriptor pipelineDesc;
  pipelineDesc.vertexInputState = Vertex::sLayout;
  pipelineDesc.textureStages = span {&textureStage, 1};
  pipelineDesc.primitiveType = PrimitiveType::TriangleStrip;
  pipelineDesc.cullMode = CullMode::CounterClockwise;
  pipelineDesc.depthTest = TestPassCond::IfLessEqual;
  pipelineDesc.depthWriteEnable = true;
  pipelineDesc.dithering = true;
  pipelineDesc.fogType = mFogType;
  pipelineDesc.fogMode = mFogMode;
  mPipeline = mGfxCache->create_pipeline(pipelineDesc);
}
auto Fog::render_ui() -> void {
  ImGui::SetNextWindowSize({300.0f, 0}, ImGuiCond_FirstUseEver);
  if (!ImGui::Begin("Fog Settings")) {
    ImGui::End();
    return;
  }

  if (ImGui::RadioButton("vertex fog", mFogType == FogType::Vertex)) {
    mFogType = FogType::Vertex;
    update_pipeline();
  }
  if (ImGui::RadioButton("vertex fog (range-based)",
                         mFogType == FogType::VertexRangeBased)) {
    mFogType = FogType::VertexRangeBased;
    update_pipeline();
  }
  if (ImGui::RadioButton("fragment fog", mFogType == FogType::Fragment)) {
    mFogType = FogType::Fragment;
    update_pipeline();
  }

  ImGui::Separator();

  DebugUi::edit_color3("Fog color", mFogColor);

  ImGui::Separator();

  if (ImGui::RadioButton("linear", mFogMode == FogMode::Linear)) {
    mFogMode = FogMode::Linear;
    update_pipeline();
  }
  if (ImGui::RadioButton("exponential", mFogMode == FogMode::Exponential)) {
    mFogMode = FogMode::Exponential;
    update_pipeline();
  }
  if (ImGui::RadioButton("exponential squared",
                         mFogMode == FogMode::ExponentialSquared)) {
    mFogMode = FogMode::ExponentialSquared;
    update_pipeline();
  }

  ImGui::BeginDisabled(mFogMode != FogMode::Linear);
  ImGui::DragFloatRange2("range", &mFogStart, &mFogEnd, 1, 0.0f, 100.0f);
  ImGui::EndDisabled();

  ImGui::BeginDisabled(mFogMode == FogMode::Linear);
  ImGui::DragFloat("density", &mFogDensity, 0.005f, 0.0f, 1.0f, "%.3f",
                   ImGuiSliderFlags_AlwaysClamp);
  ImGui::EndDisabled();

  ImGui::End();
}

auto Fog::on_update(UpdateContext& ctx) -> void {
  render_ui();

  CommandList cmdList;
  cmdList.clear_attachments(
    Attachments {Attachment::RenderTarget, Attachment::DepthBuffer}, mFogColor,
    1.0f);

  cmdList.bind_pipeline(mPipeline);
  cmdList.bind_sampler(mSampler);
  cmdList.bind_vertex_buffer(mVertexBuffer);

  const DrawContext drawCtx {ctx.drawCtx};

  const f32 aspectRatio {drawCtx.viewport.aspect_ratio()};
  const auto viewToClip {
    Matrix4x4f32::perspective_projection(120_deg, aspectRatio, 0.1f, 100.0f)};
  cmdList.set_transform(TransformState::ViewToClip, viewToClip);
  cmdList.set_transform(TransformState::WorldToView, Matrix4x4f32::identity());

  // draw signs
  for (i32 i {0}; i < 5; i++) {
    const auto localToWorld {
      Matrix4x4f32::scaling(7.5f) *
      Matrix4x4f32::translation(-10.0f, 0.0f, static_cast<f32>(i + 1) * 10.0f) *
      Matrix4x4f32::translation(static_cast<f32>(i * i) * 5.0f, 0.0f, 0.0f)};

    cmdList.set_transform(TransformState::LocalToWorld, localToWorld);
    cmdList.bind_texture(mTextures[i]);
    cmdList.draw(0, 4);
  }

  // draw ground
  const auto localToWorld {Matrix4x4f32::scaling(75.0f) *
                           Matrix4x4f32::rotation_x(90_deg) *
                           Matrix4x4f32::translation(0.0f, -7.5f, 75.0f)};
  cmdList.set_transform(TransformState::LocalToWorld, localToWorld);
  cmdList.bind_texture(mTextures[5]);
  cmdList.set_fog_parameters(mFogColor, mFogStart, mFogEnd, mFogDensity);
  cmdList.draw(0, 4);

  drawCtx.commandLists.emplace_back(std::move(cmdList));
}

} // namespace tribase
