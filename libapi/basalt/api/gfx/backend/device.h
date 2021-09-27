#pragma once

#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <gsl/span>

#include <cstddef>
#include <filesystem>
#include <optional>

namespace basalt::gfx {

struct Device {
  Device(const Device&) = delete;
  Device(Device&&) = delete;

  virtual ~Device() noexcept = default;

  auto operator=(const Device&) -> Device& = delete;
  auto operator=(Device&&) -> Device& = delete;

  // TODO: noexcept method to validate the layout for the device requirements
  //       bool Device::ValidateVertexLayout(const VertexLayout&)
  //       Writes problematic vertex elements to log. Should it also change the
  //       layout (non const ref) to the closest valid layout?
  //       (what does "closest" mean?)
  //       and / or method to validate a single vertex element (usage and type)
  //       or an API to get a list of supported usages and types

  // TODO: method to retrieve a preferred vertex layout ?

  virtual auto create_vertex_buffer(gsl::span<const std::byte> data,
                                    const VertexLayout&) -> VertexBuffer = 0;

  // TODO: load file somewhere else
  virtual auto load_texture(const std::filesystem::path&) -> Texture = 0;

  virtual auto create_sampler(const SamplerDescriptor&) -> Sampler = 0;

  virtual auto query_extension(ext::ExtensionId)
    -> std::optional<ext::ExtensionPtr> = 0;

protected:
  Device() noexcept = default;
};

} // namespace basalt::gfx
