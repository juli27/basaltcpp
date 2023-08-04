#pragma once

#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/shared/color.h>

#include <basalt/api/math/matrix4x4.h>

#include <basalt/api/base/enum_array.h>
#include <basalt/api/base/types.h>

#include <optional>

namespace basalt::gfx {

struct DeviceStateCache final {
  DeviceStateCache() noexcept = default;

  auto update(Pipeline) noexcept -> bool;
  auto update(VertexBuffer, u64 offset) noexcept -> bool;
  auto update(IndexBuffer) noexcept -> bool;
  auto update(Sampler) noexcept -> bool;
  auto update(Texture) noexcept -> bool;
  auto update(TransformState, const Matrix4x4f32&) noexcept -> bool;
  auto update_ambient_light(const Color&) noexcept -> bool;
  auto update(const Color& diffuse, const Color& ambient, const Color& emissive,
              const Color& specular, f32 specularPower) noexcept -> bool;
  auto update_fog_parameters(const Color&, f32 start, f32 end, f32 density)
    -> bool;
  auto update_blend_constant(const Color&) -> bool;
  auto update_reference_alpha(u8) -> bool;

private:
  struct Material final {
    Color diffuse;
    Color ambient;
    Color emissive;
    Color specular;
    f32 specularPower {};
  };

  struct FogParams final {
    Color color;
    f32 start {};
    f32 end {};
    f32 density {};
  };

  using MaybeMatrix = std::optional<Matrix4x4f32>;

  EnumArray<TransformState, MaybeMatrix, TRANSFORM_STATE_COUNT> mTransforms {
    {TransformState::ViewToClip, std::nullopt},
    {TransformState::WorldToView, std::nullopt},
    {TransformState::LocalToWorld, std::nullopt},
    {TransformState::Texture, std::nullopt},
  };
  std::optional<Color> mAmbientLight;
  std::optional<Material> mMaterial;
  std::optional<FogParams> mFogParams;
  std::optional<Color> mBlendConstant;
  std::optional<u8> mReferenceAlpha;
  Pipeline mBoundPipeline {Pipeline::null()};
  VertexBuffer mBoundVertexBuffer {VertexBuffer::null()};
  u64 mVertexBufferOffset {0ull};
  IndexBuffer mBoundIndexBuffer {IndexBuffer::null()};
  Sampler mBoundSampler {Sampler::null()};
  Texture mBoundTexture {Texture::null()};
};

} // namespace basalt::gfx
