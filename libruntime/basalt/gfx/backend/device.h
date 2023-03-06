#pragma once

#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <gsl/span>

#include <cstddef>
#include <filesystem>
#include <optional>

namespace basalt::gfx {

class Device {
public:
  Device(const Device&) = delete;
  Device(Device&&) = delete;

  virtual ~Device() noexcept = default;

  auto operator=(const Device&) -> Device& = delete;
  auto operator=(Device&&) -> Device& = delete;

  [[nodiscard]] virtual auto capabilities() const -> const DeviceCaps& = 0;

  [[nodiscard]] virtual auto create_pipeline(const PipelineDescriptor&)
    -> Pipeline = 0;

  virtual auto destroy(Pipeline) noexcept -> void = 0;

  // TODO: noexcept method to validate the layout for the device requirements
  //       bool Device::ValidateVertexLayout(const VertexLayout&)
  //       Writes problematic vertex elements to log. Should it also change the
  //       layout (non const ref) to the closest valid layout?
  //       (what does "closest" mean?)
  //       and / or method to validate a single vertex element (usage and type)
  //       or an API to get a list of supported usages and types

  // TODO: method to retrieve a preferred vertex layout ?

  // throws std::bad_alloc
  [[nodiscard]] virtual auto
  create_vertex_buffer(const VertexBufferDescriptor&,
                       gsl::span<const std::byte> initialData = {})
    -> VertexBuffer = 0;

  virtual auto destroy(VertexBuffer) noexcept -> void = 0;

  // offset = 0 && size = 0 maps entire buffer
  // size = 0 maps from offset until the end of the buffer
  // can return empty span
  // TODO: come up with a safer API
  [[nodiscard]] virtual auto map(VertexBuffer, uDeviceSize offset = 0,
                                 uDeviceSize size = 0)
    -> gsl::span<std::byte> = 0;

  virtual auto unmap(VertexBuffer) noexcept -> void = 0;

  // throws std::bad_alloc
  [[nodiscard]] virtual auto
  create_index_buffer(const IndexBufferDescriptor&,
                      gsl::span<const std::byte> initialData = {})
    -> IndexBuffer = 0;

  virtual auto destroy(IndexBuffer) noexcept -> void = 0;

  // offsetInBytes = 0 && sizeInBytes = 0 maps entire buffer
  // sizeInBytes = 0 maps from offsetInBytes until the end of the buffer
  // can return empty span
  // TODO: come up with a safer API
  [[nodiscard]] virtual auto map(IndexBuffer, uDeviceSize offsetInBytes = 0,
                                 uDeviceSize sizeInBytes = 0)
    -> gsl::span<std::byte> = 0;

  virtual auto unmap(IndexBuffer) noexcept -> void = 0;

  // TODO: load file somewhere else
  // throws std::runtime_error when failing
  [[nodiscard]] virtual auto load_texture(const std::filesystem::path&)
    -> Texture = 0;

  virtual auto destroy(Texture) noexcept -> void = 0;

  [[nodiscard]] virtual auto create_sampler(const SamplerDescriptor&)
    -> Sampler = 0;

  virtual auto destroy(Sampler) noexcept -> void = 0;

  virtual auto query_extension(ext::ExtensionId)
    -> std::optional<ext::ExtensionPtr> = 0;

protected:
  Device() noexcept = default;
};

} // namespace basalt::gfx
