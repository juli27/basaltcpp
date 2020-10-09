#pragma once

#include "types.h"
#include "ext/types.h"

#include "api/shared/types.h"

#include <optional>
#include <string_view>

namespace basalt::gfx {

struct Device {
  Device(const Device&) = delete;
  Device(Device&&) = delete;

  virtual ~Device() noexcept = default;

  auto operator=(const Device&) -> Device& = delete;
  auto operator=(Device &&) -> Device& = delete;

  // TODO: noexcept method to validate the layout for the device requirements
  //       bool Device::ValidateVertexLayout(const VertexLayout&)
  //       Writes problematic vertex elements to log. Should it also change the
  //       layout (non const ref) to the closest valid layout?
  //       (what does "closest" mean?)
  //       and / or method to validate a single vertex element (usage and type)
  //       or an API to get a list of supported usages and types

  // TODO: method to retrieve a preferred vertex layout ?

  /**
   * \brief Adds a static mesh to the device to prepare it for rendering.
   *
   * \param data vertex data
   * \param numVertices number of vertices
   * \param layout the layout of a vertex
   * \param primitiveType the primitive type of the mesh
   * \return handle of the added mesh
   */
  virtual auto add_mesh(void* data, i32 numVertices, const VertexLayout& layout,
                        PrimitiveType primitiveType) -> MeshHandle = 0;

  // takes in a file path for now
  // TODO: move file loading into the resources namespace
  virtual auto add_texture(std::string_view filePath) -> TextureHandle = 0;

  virtual auto query_extension(ext::ExtensionId)
    -> std::optional<ext::ExtensionPtr> = 0;

protected:
  Device() noexcept = default;
};

} // namespace basalt::gfx
