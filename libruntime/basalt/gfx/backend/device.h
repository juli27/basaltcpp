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

  [[nodiscard]] virtual auto create_pipeline(PipelineDescriptor const&)
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
  create_vertex_buffer(VertexBufferDescriptor const&,
                       gsl::span<std::byte const> initialData = {})
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
  create_index_buffer(IndexBufferDescriptor const&,
                      gsl::span<std::byte const> initialData = {})
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
  [[nodiscard]] virtual auto load_texture(std::filesystem::path const&)
    -> Texture = 0;

  [[nodiscard]] virtual auto load_cube_texture(std::filesystem::path const&)
    -> Texture = 0;

  virtual auto destroy(Texture) noexcept -> void = 0;

  [[nodiscard]] virtual auto create_sampler(SamplerDescriptor const&)
    -> Sampler = 0;

  virtual auto destroy(Sampler) noexcept -> void = 0;

  virtual auto submit(gsl::span<CommandList const>) -> void = 0;

protected:
  Device() noexcept = default;
};

} // namespace basalt::gfx
