#pragma once

#include "device.h"

#include "types.h"
#include "ext/types.h"

#include <basalt/api/gfx/backend/vertex_layout.h>

#include <basalt/api/shared/handle_pool.h>

#include <vector>

namespace basalt::gfx {

class ValidatingDevice final : public Device {
public:
  static auto wrap(DevicePtr) -> ValidatingDevicePtr;

  // don't use directly
  explicit ValidatingDevice(DevicePtr);

  auto wrap_extensions(ext::DeviceExtensions&) -> void;

  [[nodiscard]]
  auto construct_texture(TextureHandle original) -> TextureHandle;

  [[nodiscard]]
  auto capabilities() const -> DeviceCaps const& override;

  [[nodiscard]]
  auto get_status() const noexcept -> DeviceStatus override;

  auto reset() -> void override;

  [[nodiscard]]
  auto create_pipeline(PipelineCreateInfo const&) -> PipelineHandle override;

  auto destroy(PipelineHandle) noexcept -> void override;

  [[nodiscard]]
  auto create_vertex_buffer(VertexBufferCreateInfo const&)
    -> VertexBufferHandle override;

  auto destroy(VertexBufferHandle) noexcept -> void override;

  [[nodiscard]]
  auto map(VertexBufferHandle, uDeviceSize offsetInBytes,
           uDeviceSize sizeInBytes) -> gsl::span<std::byte> override;

  auto unmap(VertexBufferHandle) noexcept -> void override;

  [[nodiscard]]
  auto create_index_buffer(IndexBufferCreateInfo const&)
    -> IndexBufferHandle override;

  auto destroy(IndexBufferHandle) noexcept -> void override;

  [[nodiscard]]
  auto map(IndexBufferHandle, uDeviceSize offsetInBytes,
           uDeviceSize sizeInBytes) -> gsl::span<std::byte> override;

  auto unmap(IndexBufferHandle) noexcept -> void override;

  [[nodiscard]]
  auto load_texture(std::filesystem::path const&) -> TextureHandle override;

  [[nodiscard]]
  auto load_cube_texture(std::filesystem::path const&)
    -> TextureHandle override;

  auto destroy(TextureHandle) noexcept -> void override;

  [[nodiscard]]
  auto create_sampler(SamplerCreateInfo const&) -> SamplerHandle override;

  auto destroy(SamplerHandle) noexcept -> void override;

  auto submit(gsl::span<CommandList const>) -> void override;

private:
  struct PipelineData final {
    VertexLayoutVector vertexInputLayout;
    PrimitiveType primitiveType;
  };

  struct VertexBufferData final {
    VertexLayoutVector layout;
    uDeviceSize sizeInBytes;
  };

  struct IndexBufferData final {
    uDeviceSize sizeInBytes;
  };

  struct TextureData final {};

  struct SamplerData final {};

  DevicePtr mDevice;
  DeviceCaps mCaps;
  HandlePool<PipelineData, PipelineHandle> mPipelines;
  HandlePool<VertexBufferData, VertexBufferHandle> mVertexBuffers;
  HandlePool<IndexBufferData, IndexBufferHandle> mIndexBuffers;
  HandlePool<TextureData, TextureHandle> mTextures;
  HandlePool<SamplerData, SamplerHandle> mSamplers;
  PipelineHandle mBoundPipeline;

  PrimitiveType mCurrentPrimitiveType{PrimitiveType::PointList};

  auto validate(CommandList const&) -> void;
  auto validate(Command const&) -> void;
  auto validate(CommandClearAttachments const&) -> void;
  auto validate(CommandDraw const&) -> void;
  auto validate(CommandDrawIndexed const&) -> void;
  auto validate(CommandBindPipeline const&) -> void;
  auto validate(CommandBindVertexBuffer const&) -> void;
  auto validate(CommandBindIndexBuffer const&) -> void;
  auto validate(CommandBindSampler const&) -> void;
  auto validate(CommandBindTexture const&) -> void;
  auto validate(CommandSetTransform const&) -> void;
  auto validate(CommandSetAmbientLight const&) -> void;
  auto validate(CommandSetLights const&) -> void;
  auto validate(CommandSetMaterial const&) -> void;
  auto validate(CommandSetFogParameters const&) -> void;
};

} // namespace basalt::gfx
