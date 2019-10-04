#pragma once
#ifndef BS_GFX_BACKEND_IRENDERER_H
#define BS_GFX_BACKEND_IRENDERER_H

#include <basalt/common/Color.h>
#include <basalt/math/Mat4.h>

#include "RenderCommand.h"
#include "Types.h"

#include <string>

namespace basalt::gfx::backend {

struct IRenderer {
  IRenderer() = default;
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

  /**
   * \brief Adds a static mesh to the renderer to prepare it for rendering.
   *
   * \param data vertex data
   * \param numVertices number of vertices
   * \param layout the layout of a vertex
   * \param primitiveType the primitive type of the mesh
   * \return handle of the added mesh
   */
  virtual auto add_mesh(
    void* data, i32 numVertices, const VertexLayout& layout,
    PrimitiveType primitiveType
  ) -> MeshHandle = 0;

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

  /*
   * Adds a command to the default command buffer.
   */
  virtual void submit(const RenderCommand& command) = 0;

  /**
   * Sets the view and projection matrix for the default command buffer.
   */
  virtual void set_view_proj(
    const math::Mat4f32& view, const math::Mat4f32& projection
  ) = 0;

  /*
   * Sets the lights for this renderer. Lights apply  and
   * valid for every frame until changed again.
   *
   * TODO: move lights into the command buffer ?
   */
  virtual void set_lights(const LightSetup& lights) = 0;

  /**
   * Sets the back buffer clear color.
   */
  virtual void set_clear_color(Color color) = 0;

  virtual void render() = 0;
  virtual auto get_name() -> std::string_view = 0;

  virtual void new_gui_frame() = 0;
};

} // namespace basalt::gfx::backend

#endif // !BS_GFX_BACKEND_IRENDERER_H
