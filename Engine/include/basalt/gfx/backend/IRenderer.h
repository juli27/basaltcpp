#pragma once
#ifndef BS_GFX_BACKEND_IRENDERER_H
#define BS_GFX_BACKEND_IRENDERER_H

#include <string>

#include <basalt/math/Mat4.h>

#include "RenderCommand.h"
#include "Types.h"

namespace basalt {
namespace gfx {
namespace backend {


class IRenderer {
public:
  virtual ~IRenderer() = default;

  // TODO: noexcept method to validate the layout for the renderers requirements
  //       bool IRenderer::ValidateVertexLayout(const VertexLayout&)
  //       Writes problematic vertex elements to log. Should it also change the
  //       layout (non const ref) to the closest valid layout?
  //       (what does "closest" mean?)
  //       and / or method to validate a single vertex element (usage and type)
  //       or an API to get a list of supported usages and types

  // TODO: method to retrieve a preferred vertex layout ?

  // memory for cpu data is managed externally
  virtual MeshHandle AddMesh(
    void* data, i32 numVertices, const VertexLayout& layout,
    PrimitiveType primitiveType
  ) = 0;

  // takes in a file path for now
  // TODO: move file loading into the resources namespace
  virtual TextureHandle AddTexture(std::string_view filePath) = 0;

  // adds the command to the default command buffer
  virtual void Submit(const RenderCommand& command) = 0;

  // TODO: ugh, copies the whole command buffer every time...
  virtual void Submit(const RenderCommandBuffer& commands) = 0;

  virtual void SetViewProj(
    const math::Mat4f32& view, const math::Mat4f32& projection
  ) = 0;

  virtual void SetLights(const LightSetup& lights) = 0;

  virtual void Render() = 0;

  virtual void Present() = 0;

  virtual std::string_view GetName() = 0;
};

} // namespace backend
} // namespace gfx
} // namespace basalt

#endif // !BS_GFX_BACKEND_IRENDERER_H
