#pragma once
#ifndef BS_GFX_BACKEND_D3D9_RENDERER_H
#define BS_GFX_BACKEND_D3D9_RENDERER_H

#include <vector>

#include <basalt/gfx/backend/IRenderer.h>

#include "D3D9Header.h"

namespace basalt {
namespace gfx {
namespace backend {
namespace d3d9 {

struct RenderMesh {
  IDirect3DVertexBuffer9* vertexBuffer;
  DWORD fvf;
  UINT vertexSize;
  D3DPRIMITIVETYPE primType;
  UINT primCount;
};


class Renderer final : public IRenderer {
public:
  Renderer(IDirect3DDevice9* device);
  virtual ~Renderer();

  virtual RenderMeshHandle AddMesh(const VertexData& vertices) override;

  virtual void Submit(const RenderCommand& command) override;

  virtual void SetLights(const LightSetup& lights) override;

  virtual void Render() override;
  virtual void Present() override;

  virtual std::string_view GetName() override;

private:
  IDirect3DDevice9* m_device;
  std::vector<RenderMesh> m_meshes;
  std::vector<RenderCommand> m_commandQueue;
};

} // namespace d3d9
} // namespace backend
} // namespace gfx
} // namespace basalt

#endif // !BS_GFX_BACKEND_D3D9_RENDERER_H
