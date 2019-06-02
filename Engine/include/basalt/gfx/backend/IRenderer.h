#pragma once
#ifndef BS_GFX_BACKEND_IRENDERER_H
#define BS_GFX_BACKEND_IRENDERER_H

#include <string>

#include <basalt/common/Color.h>
#include <basalt/math/Mat4.h>

#include "Lighting.h"
#include "RenderMesh.h"

namespace basalt {
namespace gfx {
namespace backend {

struct RenderCommand {
  MeshHandle mesh;

  // TODO: Material
  Color diffuseColor;
  Color ambientColor;
  Color emissiveColor;

  math::Mat4f32 world;
};


class IRenderer {
public:
  virtual ~IRenderer() = default;

  // memory for cpu data is managed externally
  virtual MeshHandle AddMesh(const VertexData& vertices) = 0;

  virtual void Submit(const RenderCommand& command) = 0;

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
