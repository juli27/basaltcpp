#pragma once
#ifndef BASALT_GFX_BACKEND_IRENDERER_H
#define BASALT_GFX_BACKEND_IRENDERER_H

#include "runtime/gfx/backend/Types.h"

#include "runtime/shared/Size2D.h"
#include "runtime/shared/Types.h"

#include <string>

namespace basalt {

struct Color;

}

namespace basalt::gfx::backend {

struct RenderCommandList;

struct IRenderer {
  IRenderer(const IRenderer&) = delete;
  IRenderer(IRenderer&&) = delete;

  virtual ~IRenderer() = default;

  auto operator=(const IRenderer&) -> IRenderer& = delete;
  auto operator=(IRenderer&&) -> IRenderer& = delete;

  // TODO: noexcept method to validate the layout for the renderers requirements
  //       bool IRenderer::ValidateVertexLayout(const VertexLayout&)
  //       Writes problematic vertex elements to log. Should it also change the
  //       layout (non const ref) to the closest valid layout?
  //       (what does "closest" mean?)
  //       and / or method to validate a single vertex element (usage and type)
  //       or an API to get a list of supported usages and types

  // TODO: method to retrieve a preferred vertex layout ?

  virtual void on_window_resize(Size2Du16 size) = 0;

  /**
   * \brief Adds a static mesh to the renderer to prepare it for rendering.
   *
   * \param data vertex data
   * \param numVertices number of vertices
   * \param layout the layout of a vertex
   * \param primitiveType the primitive type of the mesh
   * \return handle of the added mesh
   */
  virtual auto add_mesh(void* data, i32 numVertices, const VertexLayout& layout
                      , PrimitiveType primitiveType) -> MeshHandle = 0;

  /**
   * \brief Removes a static mesh from the renderer which makes it unavailable
   *        for renderering.
   *
   * \param meshHandle handle to mesh to be removed
   */
  virtual void remove_mesh(MeshHandle meshHandle) = 0;

  // takes in a file path for now
  // TODO: move file loading into the resources namespace
  virtual auto add_texture(std::string_view filePath) -> TextureHandle = 0;

  /**
   * \brief Removes a static texture from the renderer which makes it
   *        unavailable for renderering.
   *
   * \param textureHandle handle to texture to be removed
   */
  virtual void remove_texture(TextureHandle textureHandle) = 0;

  /**
   * Sets the back buffer clear color.
   */
  virtual void set_clear_color(Color color) = 0;

  virtual void render(const RenderCommandList&) = 0;

  virtual auto name() -> std::string_view = 0;

  virtual void new_gui_frame() = 0;

protected:
    IRenderer() = default;
};

} // namespace basalt::gfx::backend

#endif // !BASALT_GFX_BACKEND_IRENDERER_H
