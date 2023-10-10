#pragma once

#include <basalt/gfx/backend/device.h>

#include <basalt/gfx/backend/types.h>
#include <basalt/gfx/backend/ext/types.h>

#include <basalt/api/shared/handle_pool.h>

#include <vector>

namespace basalt::gfx {

class ValidatingDevice final : public Device {
  struct TextureData;

public:
  static auto wrap(DevicePtr) -> ValidatingDevicePtr;

  // don't use directly
  explicit ValidatingDevice(DevicePtr);

  auto wrap_extensions(ext::DeviceExtensions&) -> void;

  [[nodiscard]]
  auto construct_texture(Texture original) -> Texture;

  [[nodiscard]]
  auto capabilities() const -> DeviceCaps const& override;

  [[nodiscard]]
  auto get_status() const noexcept -> DeviceStatus override;

  auto reset() -> void override;

  [[nodiscard]]
  auto create_pipeline(PipelineDescriptor const&) -> Pipeline override;

  auto destroy(Pipeline) noexcept -> void override;

  [[nodiscard]]
  auto create_vertex_buffer(VertexBufferDescriptor const&,
                            gsl::span<std::byte const> initialData)
    -> VertexBuffer override;

  auto destroy(VertexBuffer) noexcept -> void override;

  [[nodiscard]]
  auto map(VertexBuffer, uDeviceSize offset, uDeviceSize size)
    -> gsl::span<std::byte> override;

  auto unmap(VertexBuffer) noexcept -> void override;

  [[nodiscard]]
  auto create_index_buffer(IndexBufferDescriptor const&,
                           gsl::span<std::byte const> initialData)
    -> IndexBuffer override;

  auto destroy(IndexBuffer) noexcept -> void override;

  [[nodiscard]]
  auto map(IndexBuffer, uDeviceSize offsetInBytes, uDeviceSize sizeInBytes)
    -> gsl::span<std::byte> override;

  auto unmap(IndexBuffer) noexcept -> void override;

  [[nodiscard]]
  auto load_texture(std::filesystem::path const&) -> Texture override;

  [[nodiscard]]
  auto load_cube_texture(std::filesystem::path const&) -> Texture override;

  auto destroy(Texture) noexcept -> void override;

  [[nodiscard]]
  auto create_sampler(SamplerDescriptor const&) -> Sampler override;

  auto destroy(Sampler) noexcept -> void override;

  auto submit(gsl::span<CommandList const>) -> void override;

private:
  struct PipelineData final {
    Pipeline originalId;
    std::vector<VertexElement> vertexInputLayout;
    PrimitiveType primitiveType;
  };

  struct VertexBufferData final {
    VertexBuffer originalId;
    std::vector<VertexElement> layout;
    uDeviceSize sizeInBytes;
  };

  struct IndexBufferData final {
    IndexBuffer originalId;
    uDeviceSize sizeInBytes;
  };

  struct TextureData final {
    Texture originalId;
  };

  struct SamplerData final {
    Sampler originalId;
  };

  DevicePtr mDevice;
  DeviceCaps mCaps;
  HandlePool<PipelineData, Pipeline> mPipelines;
  HandlePool<VertexBufferData, VertexBuffer> mVertexBuffers;
  HandlePool<IndexBufferData, IndexBuffer> mIndexBuffers;
  HandlePool<TextureData, Texture> mTextures;
  HandlePool<SamplerData, Sampler> mSamplers;
  Pipeline mBoundPipeline;

  PrimitiveType mCurrentPrimitiveType{PrimitiveType::PointList};

  auto validate(CommandList const&) -> CommandList;
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

  auto patch(CommandList&, Command const&) -> void;
  auto patch(CommandList&, CommandClearAttachments const&) -> void;
  auto patch(CommandList&, CommandDraw const&) -> void;
  auto patch(CommandList&, CommandDrawIndexed const&) -> void;
  auto patch(CommandList&, CommandBindPipeline const&) -> void;
  auto patch(CommandList&, CommandBindVertexBuffer const&) -> void;
  auto patch(CommandList&, CommandBindIndexBuffer const&) -> void;
  auto patch(CommandList&, CommandBindSampler const&) -> void;
  auto patch(CommandList&, CommandBindTexture const&) -> void;
  auto patch(CommandList&, CommandSetStencilReference const&) -> void;
  auto patch(CommandList&, CommandSetStencilReadMask const&) -> void;
  auto patch(CommandList&, CommandSetStencilWriteMask const&) -> void;
  auto patch(CommandList&, CommandSetBlendConstant const&) -> void;
  auto patch(CommandList&, CommandSetTransform const&) -> void;
  auto patch(CommandList&, CommandSetAmbientLight const&) -> void;
  auto patch(CommandList&, CommandSetLights const&) -> void;
  auto patch(CommandList&, CommandSetMaterial const&) -> void;
  auto patch(CommandList&, CommandSetFogParameters const&) -> void;
  auto patch(CommandList&, CommandSetReferenceAlpha const&) -> void;
  auto patch(CommandList&, CommandSetTextureFactor const&) -> void;
  auto patch(CommandList&, CommandSetTextureStageConstant const&) -> void;
};

} // namespace basalt::gfx
