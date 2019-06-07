#pragma once
#ifndef BS_GFX_BACKEND_D3D9_RENDERER_H
#define BS_GFX_BACKEND_D3D9_RENDERER_H

#include <basalt/gfx/backend/IRenderer.h>

#include <vector>

#include <basalt/gfx/backend/Lighting.h>
#include <basalt/gfx/backend/RenderCommand.h>
#include <basalt/gfx/backend/RenderMesh.h>

#include "D3D9Header.h"

namespace basalt {
namespace gfx {
namespace backend {
namespace d3d9 {


struct Mesh {
  IDirect3DVertexBuffer9* vertexBuffer;
  MeshHandle handle;
  DWORD fvf;
  UINT vertexSize;
  D3DPRIMITIVETYPE primType;
  UINT primCount;
};


struct Texture {
  TextureHandle handle;
  IDirect3DTexture9* texture;
};


class Renderer final : public IRenderer {
public:
  Renderer(IDirect3DDevice9* device);
  virtual ~Renderer();

  virtual MeshHandle AddMesh(const VertexData& vertices) override;

  virtual TextureHandle AddTexture(std::string_view filePath) override;

  virtual void Submit(const RenderCommand& command) override;

  virtual void Submit(const RenderCommandBuffer& commands) override;

  virtual void SetViewProj(
    const math::Mat4f32& view, const math::Mat4f32& projection
  ) override;

  virtual void SetLights(const LightSetup& lights) override;

  virtual void Render() override;
  virtual void Present() override;

  virtual std::string_view GetName() override;

private:
  void RenderCommands(const RenderCommandBuffer& commands);

private:
  IDirect3DDevice9* m_device;
  std::vector<Mesh> m_meshes;
  std::vector<Texture> m_textures;
  std::vector<RenderCommandBuffer> m_commandBuffers;
};

} // namespace d3d9
} // namespace backend
} // namespace gfx
} // namespace basalt

#endif // !BS_GFX_BACKEND_D3D9_RENDERER_H
