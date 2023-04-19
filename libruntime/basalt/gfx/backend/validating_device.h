#pragma once

#include <basalt/gfx/backend/device.h>

#include <basalt/gfx/backend/types.h>

#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/shared/handle_pool.h>

#include <vector>

namespace basalt::gfx {

class ValidatingDevice final : public Device {
public:
  static auto wrap(DevicePtr) -> ValidatingDevicePtr;

  // don't use directly
  explicit ValidatingDevice(DevicePtr);

  auto validate(const CommandList&) -> CommandList;

  [[nodiscard]] auto capabilities() const -> const DeviceCaps& override;

  [[nodiscard]] auto create_pipeline(const PipelineDescriptor&)
    -> Pipeline override;

  auto destroy(Pipeline) noexcept -> void override;

  [[nodiscard]] auto
  create_vertex_buffer(const VertexBufferDescriptor&,
                       gsl::span<const std::byte> initialData)
    -> VertexBuffer override;

  auto destroy(VertexBuffer) noexcept -> void override;

  [[nodiscard]] auto map(VertexBuffer, uDeviceSize offset, uDeviceSize size)
    -> gsl::span<std::byte> override;

  auto unmap(VertexBuffer) noexcept -> void override;

  [[nodiscard]] auto create_index_buffer(const IndexBufferDescriptor&,
                                         gsl::span<const std::byte> initialData)
    -> IndexBuffer override;

  auto destroy(IndexBuffer) noexcept -> void override;

  [[nodiscard]] auto map(IndexBuffer, uDeviceSize offsetInBytes,
                         uDeviceSize sizeInBytes)
    -> gsl::span<std::byte> override;

  auto unmap(IndexBuffer) noexcept -> void override;

  [[nodiscard]] auto load_texture(const std::filesystem::path&)
    -> Texture override;

  auto destroy(Texture) noexcept -> void override;

  [[nodiscard]] auto create_sampler(const SamplerDescriptor&)
    -> Sampler override;

  auto destroy(Sampler) noexcept -> void override;

  auto query_extension(ext::ExtensionId)
    -> std::optional<ext::ExtensionPtr> override;

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
  HandlePool<PipelineData, Pipeline> mPipelines;
  HandlePool<VertexBufferData, VertexBuffer> mVertexBuffers;
  HandlePool<IndexBufferData, IndexBuffer> mIndexBuffers;
  HandlePool<TextureData, Texture> mTextures;
  HandlePool<SamplerData, Sampler> mSamplers;

  PrimitiveType mCurrentPrimitiveType {PrimitiveType::PointList};

  auto validate(const Command&) -> void;
  auto validate(const CommandClearAttachments&) -> void;
  auto validate(const CommandDraw&) -> void;
  auto validate(const CommandDrawIndexed&) -> void;
  auto validate(const CommandBindPipeline&) -> void;
  auto validate(const CommandBindVertexBuffer&) -> void;
  auto validate(const CommandBindIndexBuffer&) -> void;
  auto validate(const CommandBindSampler&) -> void;
  auto validate(const CommandBindTexture&) -> void;
  auto validate(const CommandSetTransform&) -> void;
  auto validate(const CommandSetAmbientLight&) -> void;
  auto validate(const CommandSetLights&) -> void;
  auto validate(const CommandSetMaterial&) -> void;

  auto patch(CommandList&, const Command&) -> void;
  auto patch(CommandList&, const CommandClearAttachments&) -> void;
  auto patch(CommandList&, const CommandDraw&) -> void;
  auto patch(CommandList&, const CommandDrawIndexed&) -> void;
  auto patch(CommandList&, const CommandBindPipeline&) -> void;
  auto patch(CommandList&, const CommandBindVertexBuffer&) -> void;
  auto patch(CommandList&, const CommandBindIndexBuffer&) -> void;
  auto patch(CommandList&, const CommandBindSampler&) -> void;
  auto patch(CommandList&, const CommandBindTexture&) -> void;
  auto patch(CommandList&, const CommandSetTransform&) -> void;
  auto patch(CommandList&, const CommandSetAmbientLight&) -> void;
  auto patch(CommandList&, const CommandSetLights&) -> void;
  auto patch(CommandList&, const CommandSetMaterial&) -> void;
};

} // namespace basalt::gfx