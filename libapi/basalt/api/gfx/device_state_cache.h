#pragma once

#include <basalt/api/gfx/backend/render_state.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/shared/color.h>

#include <basalt/api/math/matrix4x4.h>

#include <basalt/api/base/enum_array.h>
#include <basalt/api/base/types.h>

namespace basalt::gfx {

struct DeviceStateCache final {
  DeviceStateCache() noexcept = default;

  auto update(const RenderState&) noexcept -> bool;
  auto update(Pipeline) noexcept -> bool;
  auto update(VertexBuffer, u64 offset) noexcept -> bool;
  auto update(Sampler) noexcept -> bool;
  auto update(Texture) noexcept -> bool;
  auto update(TransformState, const Matrix4x4f32&) noexcept -> bool;
  auto update_ambient_light(const Color&) noexcept -> bool;
  auto update(const Color& diffuse, const Color& ambient,
              const Color& emissive) noexcept -> bool;

private:
  struct Material final {
    Color diffuse;
    Color ambient;
    Color emissive;
  };

  using RenderStateArray = EnumArray<RenderStateType, RenderStateValue, 0>;

  RenderStateArray mRenderStates {};

  EnumArray<TransformState, Matrix4x4f32, TRANSFORM_STATE_COUNT> mTransforms {
    {TransformState::ViewToViewport, Matrix4x4f32::identity()},
    {TransformState::WorldToView, Matrix4x4f32::identity()},
    {TransformState::ModelToWorld, Matrix4x4f32::identity()},
    {TransformState::Texture, Matrix4x4f32::identity()},
  };
  Color mAmbientLight;
  Material mMaterial;
  Pipeline mBoundPipeline {Pipeline::null()};
  VertexBuffer mBoundVertexBuffer {VertexBuffer::null()};
  u64 mVertexBufferOffset {0ull};
  Sampler mBoundSampler {Sampler::null()};
  Texture mBoundTexture {Texture::null()};
};

} // namespace basalt::gfx
