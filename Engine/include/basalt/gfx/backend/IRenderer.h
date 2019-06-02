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
  RenderMeshHandle mesh;

  // TODO: Material
  Color diffuseColor;
  Color ambientColor;
  Color emissiveColor;

  math::Mat4f32 world;
  math::Mat4f32 view;
  math::Mat4f32 proj;
};


class IRenderer {
public:
  virtual ~IRenderer() = default;

  // memory for cpu data is managed externally
  virtual RenderMeshHandle AddMesh(const VertexData& vertices) = 0;

  virtual void Submit(const RenderCommand& command) = 0;

  virtual void SetLights(const LightSetup& lights) = 0;

  virtual void Render() = 0;

  virtual void Present() = 0;

  virtual std::string_view GetName() = 0;
};

} // namespace backend
} // namespace gfx
} // namespace basalt

#endif // !BS_GFX_BACKEND_IRENDERER_H
