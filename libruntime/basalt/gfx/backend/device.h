#pragma once

#include <basalt/gfx/backend/types.h>
#include <basalt/gfx/backend/ext/types.h>

#include <gsl/span>

#include <cstddef>
#include <filesystem>

namespace basalt::gfx {

class Device {
public:
  Device(Device const&) = delete;
  Device(Device&&) = delete;

  virtual ~Device() noexcept = default;

  auto operator=(Device const&) -> Device& = delete;
  auto operator=(Device&&) -> Device& = delete;

  [[nodiscard]] virtual auto capabilities() const -> DeviceCaps const& = 0;
  [[nodiscard]] virtual auto get_status() const noexcept -> DeviceStatus = 0;

  virtual auto reset() -> void = 0;

  [[nodiscard]] virtual auto create_pipeline(PipelineCreateInfo const&)
    -> PipelineHandle = 0;

  virtual auto destroy(PipelineHandle) noexcept -> void = 0;

  // throws std::bad_alloc
  [[nodiscard]]
  virtual auto create_vertex_buffer(VertexBufferCreateInfo const&)
    -> VertexBufferHandle = 0;

  virtual auto destroy(VertexBufferHandle) noexcept -> void = 0;

  // offsetInBytes = 0 && sizeInBytes = 0 maps entire buffer
  // sizeInBytes = 0 maps from offsetInBytes until the end of the buffer
  // can return empty span
  // TODO: come up with a safer API
  [[nodiscard]]
  virtual auto map(VertexBufferHandle, uDeviceSize offsetInBytes = 0,
                   uDeviceSize sizeInBytes = 0) -> gsl::span<std::byte> = 0;

  virtual auto unmap(VertexBufferHandle) noexcept -> void = 0;

  // throws std::bad_alloc
  [[nodiscard]]
  virtual auto create_index_buffer(IndexBufferCreateInfo const&)
    -> IndexBufferHandle = 0;

  virtual auto destroy(IndexBufferHandle) noexcept -> void = 0;

  // offsetInBytes = 0 && sizeInBytes = 0 maps entire buffer
  // sizeInBytes = 0 maps from offsetInBytes until the end of the buffer
  // can return empty span
  // TODO: come up with a safer API
  [[nodiscard]]
  virtual auto map(IndexBufferHandle, uDeviceSize offsetInBytes = 0,
                   uDeviceSize sizeInBytes = 0) -> gsl::span<std::byte> = 0;

  virtual auto unmap(IndexBufferHandle) noexcept -> void = 0;

  // TODO: load file somewhere else
  // throws std::runtime_error when failing
  [[nodiscard]] virtual auto load_texture(std::filesystem::path const&)
    -> TextureHandle = 0;

  [[nodiscard]] virtual auto load_cube_texture(std::filesystem::path const&)
    -> TextureHandle = 0;

  virtual auto destroy(TextureHandle) noexcept -> void = 0;

  [[nodiscard]] virtual auto create_sampler(SamplerCreateInfo const&)
    -> SamplerHandle = 0;

  virtual auto destroy(SamplerHandle) noexcept -> void = 0;

  virtual auto submit(gsl::span<CommandList const>) -> void = 0;

protected:
  Device() noexcept = default;
};

} // namespace basalt::gfx
