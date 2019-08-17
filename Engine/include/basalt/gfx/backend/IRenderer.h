#pragma once
#ifndef BS_GFX_BACKEND_IRENDERER_H
#define BS_GFX_BACKEND_IRENDERER_H

#include <string>

#include <basalt/common/Color.h>
#include <basalt/math/Mat4.h>

#include "RenderCommand.h"
#include "Types.h"

namespace basalt::gfx::backend {


enum class RendererType : i8 {
  Default,
  Direct3D9,
};


class IRenderer {
public:
  inline IRenderer() = default;
  virtual ~IRenderer() = default;

  IRenderer(const IRenderer&) = delete;
  IRenderer(IRenderer&&) = delete;

public:
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

public:
  /**
   * \brief Adds a static mesh to the renderer to prepare it for rendering.
   *
   * \param data vertex data
   * \param numVertices number of vertices
   * \param layout the layout of a vertex
   * \param primitiveType the primitive type of the mesh
   * \return handle of the added mesh
   */
  virtual auto AddMesh(
    void* data, i32 numVertices, const VertexLayout& layout,
    PrimitiveType primitiveType
  ) -> MeshHandle = 0;

  /**
   * \brief Removes a static mesh from the renderer which makes it unavailable
   *        for renderering.
   *
   * \param meshHandle handle to mesh to be removed
   */
  virtual void RemoveMesh(MeshHandle meshHandle) = 0;

  // takes in a file path for now
  // TODO: move file loading into the resources namespace
  virtual auto AddTexture(std::string_view filePath) -> TextureHandle = 0;

  /**
   * \brief Removes a static texture from the renderer which makes it
   *        unavailable for renderering.
   *
   * \param textureHandle handle to texture to be removed
   */
  virtual void RemoveTexture(TextureHandle textureHandle) = 0;

  /*
   * Adds a command to the default command buffer.
   */
  virtual void Submit(const RenderCommand& command) = 0;

  /**
   * Sets the view and projection matrix for the default command buffer.
   */
  virtual void SetViewProj(
    const math::Mat4f32& view, const math::Mat4f32& projection
  ) = 0;

  /*
   * Sets the lights for this renderer. Lights apply  and
   * valid for every frame until changed again.
   *
   * TODO: move lights into the command buffer ?
   */
  virtual void SetLights(const LightSetup& lights) = 0;

  /**
   * Sets the back buffer clear color.
   */
  virtual void SetClearColor(Color color) = 0;

  virtual void Render() = 0;

  virtual void Present() = 0;

  virtual auto GetName() -> std::string_view = 0;


  virtual void NewGuiFrame() = 0;
  virtual void RenderGUI() = 0;

public:
  static auto Create(RendererType type) -> IRenderer*;
};

} // namespace basalt::gfx::backend

#endif // !BS_GFX_BACKEND_IRENDERER_H
