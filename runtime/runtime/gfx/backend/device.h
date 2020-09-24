#pragma once

#include "types.h"

#include <runtime/shared/types.h>

#include <memory>
#include <string_view>

namespace basalt::gfx {

struct Device;
using DevicePtr = std::shared_ptr<Device>;

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

  /**
   * \brief Adds a static mesh to the device to prepare it for rendering.
   *
   * \param data vertex data
   * \param numVertices number of vertices
   * \param layout the layout of a vertex
   * \param primitiveType the primitive type of the mesh
   * \return handle of the added mesh
   */
  virtual auto add_mesh(
    void* data, i32 numVertices, const VertexLayout& layout
  , PrimitiveType primitiveType) -> MeshHandle = 0;

  /**
   * \brief Removes a static mesh from the device which makes it unavailable
   *        for rendering.
   *
   * \param meshHandle handle to mesh to be removed
   */
  virtual void remove_mesh(MeshHandle meshHandle) = 0;

  // takes in a file path for now
  // TODO: move file loading into the resources namespace
  virtual auto add_texture(std::string_view filePath) -> TextureHandle = 0;

  /**
   * \brief Removes a static texture from the device which makes it
   *        unavailable for rendering.
   *
   * \param textureHandle handle to texture to be removed
   */
  virtual void remove_texture(TextureHandle textureHandle) = 0;

  virtual auto load_model(std::string_view filePath) -> ModelHandle = 0;
  virtual void remove_model(ModelHandle) = 0;

  virtual void init_dear_imgui() = 0;
  virtual void shutdown_dear_imgui() = 0;
  virtual void new_gui_frame() = 0;

protected:
    Device() noexcept = default;
};

} // namespace basalt::gfx
