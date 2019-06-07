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

  // memory for cpu data is managed externally
  virtual MeshHandle AddMesh(const VertexData& vertices) = 0;

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
