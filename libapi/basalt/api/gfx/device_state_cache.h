#pragma once

#include <basalt/api/gfx/backend/render_state.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/shared/color.h>

#include <basalt/api/math/mat4.h>

#include <basalt/api/base/enum_array.h>
#include <basalt/api/base/types.h>

namespace basalt::gfx {

struct DeviceStateCache final {
  DeviceStateCache() noexcept;

  auto update(const RenderState&) noexcept -> bool;
  auto update(Pipeline) noexcept -> bool;
  auto update(VertexBuffer, u64 offset) noexcept -> bool;
  auto update(Sampler) noexcept -> bool;
  auto update(Texture) noexcept -> bool;
  auto update(TransformState, const Mat4f32&) noexcept -> bool;
  auto update(const Color& diffuse, const Color& ambient,
              const Color& emissive) noexcept -> bool;
  auto update(TextureStageState, u32 value) noexcept -> bool;

private:
  struct Material final {
    Color diffuse;
    Color ambient;
    Color emissive;
  };

  using RenderStateArray = EnumArray<RenderStateType, RenderStateValue, 5>;

  RenderStateArray mRenderStates {
    {RenderStateType::Ambient, Color {0.0f, 0.0f, 0.0f, 0.0f}},
    {RenderStateType::FillMode, FillMode::Solid},
    {RenderStateType::DepthTest, DepthTestPass::IfLessEqual},
    {RenderStateType::DepthWrite, true},
    {RenderStateType::ShadeMode, ShadeMode::Gouraud},
  };

  EnumArray<TextureStageState, u32, TEXTURE_STAGE_STATE_COUNT> mTextureStates;
  EnumArray<TransformState, Mat4f32, TRANSFORM_STATE_COUNT> mTransforms {
    {TransformState::ViewToViewport, Mat4f32::identity()},
    {TransformState::WorldToView, Mat4f32::identity()},
    {TransformState::ModelToWorld, Mat4f32::identity()},
    {TransformState::Texture, Mat4f32::identity()},
  };
  Material mMaterial;
  Pipeline mBoundPipeline {Pipeline::null()};
  VertexBuffer mBoundVertexBuffer {VertexBuffer::null()};
  u64 mVertexBufferOffset {0ull};
  Sampler mBoundSampler {Sampler::null()};
  Texture mBoundTexture {Texture::null()};
};

} // namespace basalt::gfx
