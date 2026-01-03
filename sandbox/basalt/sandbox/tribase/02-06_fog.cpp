#include <basalt/sandbox/tribase/02-06_fog.h>

#include <basalt/api/debug_ui.h>
#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/backend/buffer.h>
#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/pipeline.h>
#include <basalt/api/gfx/backend/vertex_layout.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/matrix4x4.h>
#include <basalt/api/math/vector2.h>
#include <basalt/api/math/vector3.h>

#include <fmt/format.h>
#include <gsl/span>
#include <imgui.h>

#include <array>
#include <string>
#include <utility>

using namespace basalt::literals;

using basalt::DebugUi;
using basalt::Engine;
using basalt::Matrix4x4f32;
using basalt::Vector2f32;
using basalt::Vector3f32;
using basalt::gfx::Attachment;
using basalt::gfx::Attachments;
using basalt::gfx::CommandList;
using basalt::gfx::CullMode;
using basalt::gfx::FixedFragmentShaderCreateInfo;
using basalt::gfx::FixedVertexShaderCreateInfo;
using basalt::gfx::FogMode;
using basalt::gfx::PipelineCreateInfo;
using basalt::gfx::PrimitiveType;
using basalt::gfx::SamplerCreateInfo;
using basalt::gfx::TestPassCond;
using basalt::gfx::TextureFilter;
using basalt::gfx::TextureHandle;
using basalt::gfx::TextureMipFilter;
using basalt::gfx::TextureStage;
using basalt::gfx::TransformState;
using basalt::gfx::VertexBufferCreateInfo;
using basalt::gfx::VertexElement;

using gsl::span;

using std::array;
using std::string;

namespace tribase {

namespace {

struct Vertex final {
  Vector3f32 pos;
  ColorEncoding::A8R8G8B8 diffuse{};
  Vector2f32 uv{};

  static constexpr auto sLayout =
    basalt::gfx::make_vertex_layout<VertexElement::Position3F32,
                                    VertexElement::ColorDiffuse1U32A8R8G8B8,
                                    VertexElement::TextureCoords2F32>();
};

} // namespace

Fog::Fog(Engine const& engine)
  : mGfxCache{engine.create_gfx_resource_cache()}
  , mVertexBuffer{[&] {
    constexpr auto vertices = array{
      Vertex{{-1.0f, -1.0f, 0.0f},
             ColorEncoding::pack_a8r8g8b8_u32(191, 255, 255),
             {0.0f, 1.0f}},
      Vertex{{-1.0f, 1.0f, 0.0f},
             ColorEncoding::pack_a8r8g8b8_u32(255, 191, 255),
             {0.0f, 0.0f}},
      Vertex{{1.0f, -1.0f, 0.0f},
             ColorEncoding::pack_a8r8g8b8_u32(255, 255, 191),
             {1.0f, 1.0f}},
      Vertex{{1.0f, 1.0f, 0.0f},
             ColorEncoding::pack_a8r8g8b8_u32(255, 255, 255),
             {1.0f, 0.0f}},
    };
    auto const vertexData = as_bytes(span{vertices});

    return mGfxCache->create_vertex_buffer(
      VertexBufferCreateInfo{vertexData.size_bytes(), Vertex::sLayout},
      vertexData);
  }()}
  , mSampler{[&] {
    auto desc = SamplerCreateInfo{};
    desc.magFilter = TextureFilter::Bilinear;
    desc.minFilter = TextureFilter::Bilinear;
    desc.mipFilter = TextureMipFilter::Linear;

    return mGfxCache->create_sampler(desc);
  }()}
  , mTextures{[&] {
    auto textures = array<TextureHandle, sNumTextures>{};

    for (auto i = uSize{0}; i < sNumTextures; i++) {
      auto const fileName =
        fmt::format(FMT_STRING("data/tribase/02-06_fog/Texture{}.bmp"), i + 1);
      textures[i] = mGfxCache->load_texture_2d(fileName);
    }

    return textures;
  }()} {
  update_pipeline(engine.gfx_context());
}

auto Fog::update_pipeline(basalt::gfx::Context& gfxCtx) -> void {
  mReloadableGfxResources = gfxCtx.create_resource_cache();

  auto vs = FixedVertexShaderCreateInfo{};
  vs.fog = mFogMode;
  vs.fogRangeBased = mVertexFogRangeBased;

  auto constexpr textureStages = array{TextureStage{}};
  auto const fogMode = mFragmentFog ? mFogMode : FogMode::None;
  auto const fs = FixedFragmentShaderCreateInfo{textureStages, fogMode};

  auto desc = PipelineCreateInfo{};
  desc.vertexShader = &vs;
  desc.fragmentShader = &fs;
  desc.vertexLayout = Vertex::sLayout;
  desc.primitiveType = PrimitiveType::TriangleStrip;
  desc.cullMode = CullMode::CounterClockwise;
  desc.depthTest = TestPassCond::IfLessEqual;
  desc.depthWriteEnable = true;
  desc.dithering = true;
  mPipeline = mReloadableGfxResources->create_pipeline(desc);
}

auto Fog::render_ui(basalt::gfx::Context& gfxCtx) -> void {
  ImGui::SetNextWindowSize({300.0f, 0}, ImGuiCond_FirstUseEver);
  if (!ImGui::Begin("Fog Settings")) {
    ImGui::End();
    return;
  }

  if (ImGui::RadioButton("vertex fog",
                         !mFragmentFog && !mVertexFogRangeBased)) {
    mVertexFogRangeBased = false;
    mFragmentFog = false;
    update_pipeline(gfxCtx);
  }
  if (ImGui::RadioButton("vertex fog (range-based)",
                         !mFragmentFog && mVertexFogRangeBased)) {
    mVertexFogRangeBased = true;
    mFragmentFog = false;
    update_pipeline(gfxCtx);
  }
  if (ImGui::RadioButton("fragment fog", mFragmentFog)) {
    mFragmentFog = true;
    update_pipeline(gfxCtx);
  }

  ImGui::Separator();

  DebugUi::edit_color3("Fog color", mFogColor);

  ImGui::Separator();

  if (ImGui::RadioButton("linear", mFogMode == FogMode::Linear)) {
    mFogMode = FogMode::Linear;
    update_pipeline(gfxCtx);
  }
  if (ImGui::RadioButton("exponential", mFogMode == FogMode::Exponential)) {
    mFogMode = FogMode::Exponential;
    update_pipeline(gfxCtx);
  }
  if (ImGui::RadioButton("exponential squared",
                         mFogMode == FogMode::ExponentialSquared)) {
    mFogMode = FogMode::ExponentialSquared;
    update_pipeline(gfxCtx);
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
  render_ui(ctx.engine.gfx_context());

  auto cmdList = CommandList{};
  cmdList.clear_attachments(
    Attachments{Attachment::RenderTarget, Attachment::DepthBuffer}, mFogColor,
    1.0f);

  cmdList.bind_pipeline(mPipeline);
  cmdList.bind_sampler(0, mSampler);
  cmdList.set_fog_parameters(mFogColor, mFogStart, mFogEnd, mFogDensity);

  auto const drawCtx = ctx.drawCtx;

  auto const aspectRatio = drawCtx.viewport.aspect_ratio();
  cmdList.set_transform(
    TransformState::ViewToClip,
    Matrix4x4f32::perspective_projection(120_deg, aspectRatio, 0.1f, 100.0f));

  cmdList.set_transform(TransformState::WorldToView, Matrix4x4f32::identity());

  cmdList.bind_vertex_buffer(mVertexBuffer);

  // draw signs
  for (auto i = uSize{0}; i < 5; i++) {
    auto const localToWorld =
      Matrix4x4f32::scaling(7.5f) *
      Matrix4x4f32::translation(-10.0f, 0.0f, static_cast<f32>(i + 1) * 10.0f) *
      Matrix4x4f32::translation(static_cast<f32>(i * i) * 5.0f, 0.0f, 0.0f);
    cmdList.set_transform(TransformState::LocalToWorld, localToWorld);

    cmdList.bind_texture(0, mTextures[i]);
    cmdList.draw(0, 4);
  }

  // draw ground
  auto const localToWorld = Matrix4x4f32::scaling(75.0f) *
                            Matrix4x4f32::rotation_x(90_deg) *
                            Matrix4x4f32::translation(0.0f, -7.5f, 75.0f);
  cmdList.set_transform(TransformState::LocalToWorld, localToWorld);
  cmdList.bind_texture(0, mTextures[5]);
  cmdList.draw(0, 4);

  drawCtx.commandLists.emplace_back(std::move(cmdList));
}

} // namespace tribase
