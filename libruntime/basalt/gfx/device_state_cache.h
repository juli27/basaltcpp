#pragma once

#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/shared/color.h>

#include <basalt/api/math/matrix4.h>

#include <basalt/api/base/enum_array.h>
#include <basalt/api/base/types.h>

#include <array>
#include <optional>

namespace basalt::gfx {

struct DeviceStateCache final {
  DeviceStateCache() noexcept = default;

  auto update(PipelineHandle) noexcept -> bool;
  auto update(VertexBufferHandle, u64 offset) noexcept -> bool;
  auto update(IndexBufferHandle) noexcept -> bool;
  auto update(u8 slot, SamplerHandle) noexcept -> bool;
  auto update(u8 slot, TextureHandle) noexcept -> bool;
  auto update(TransformState, Matrix4x4f32 const&) noexcept -> bool;
  auto update_ambient_light(Color const&) noexcept -> bool;
  auto update(Color const& diffuse, Color const& ambient, Color const& emissive,
              Color const& specular, f32 specularPower) noexcept -> bool;
  auto update_fog_parameters(Color const&, f32 start, f32 end, f32 density)
    -> bool;
  auto update_blend_constant(Color const&) -> bool;
  auto update_reference_alpha(u8) -> bool;

private:
  struct Material final {
    Color diffuse;
    Color ambient;
    Color emissive;
    Color specular;
    f32 specularPower{};
  };

  struct FogParams final {
    Color color;
    f32 start{};
    f32 end{};
    f32 density{};
  };

  using MaybeMatrix = std::optional<Matrix4x4f32>;

  EnumArray<TransformState, MaybeMatrix, TRANSFORM_STATE_COUNT> mTransforms{};
  std::optional<Color> mAmbientLight;
  std::optional<Material> mMaterial;
  std::optional<FogParams> mFogParams;
  std::optional<Color> mBlendConstant;
  std::optional<u8> mReferenceAlpha;
  PipelineHandle mBoundPipeline;
  VertexBufferHandle mBoundVertexBuffer;
  u64 mVertexBufferOffset{};
  IndexBufferHandle mBoundIndexBuffer;
  std::array<SamplerHandle, 8> mBoundSamplers{};
  std::array<TextureHandle, 8> mBoundTextures{};
};

} // namespace basalt::gfx
