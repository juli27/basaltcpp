#pragma once
#ifndef BS_GFX_BACKEND_D3D9_RENDERER_H
#define BS_GFX_BACKEND_D3D9_RENDERER_H

#include <basalt/gfx/backend/IRenderer.h>

#include <basalt/common/HandlePool.h>

#include "D3D9Header.h"

namespace basalt::gfx::backend::d3d9 {


struct Mesh {
  IDirect3DVertexBuffer9* vertexBuffer;
  DWORD fvf;
  UINT vertexSize;
  D3DPRIMITIVETYPE primType;
  UINT primCount;
};


struct Texture {
  IDirect3DTexture9* texture;
};


class Renderer final : public IRenderer {
public:

  Renderer(IDirect3DDevice9* device);


  virtual ~Renderer();

  Renderer() = delete;
  Renderer(const Renderer&) = delete;
  Renderer(Renderer&&) = delete;

public:
  virtual auto AddMesh(
    void* data, i32 numVertices, const VertexLayout& layout,
    PrimitiveType primitiveType
  ) -> MeshHandle override;
  virtual void RemoveMesh(MeshHandle meshHandle) override;
  virtual auto AddTexture(std::string_view filePath) -> TextureHandle override;
  virtual void RemoveTexture(TextureHandle textureHandle) override;
  virtual void Submit(const RenderCommand& command) override;
  virtual void SetViewProj(
    const math::Mat4f32& view, const math::Mat4f32& projection
  ) override;
  virtual void SetLights(const LightSetup& lights) override;
  virtual void SetClearColor(Color color) override;
  virtual void Render() override;
  virtual void Present() override;
  virtual auto GetName() -> std::string_view override;

  virtual void NewGuiFrame() override;
  virtual void RenderGUI() override;

public:
  auto operator=(const Renderer&) -> Renderer& = delete;
  auto operator=(Renderer&&) -> Renderer& = delete;

private:

  void RenderCommands(const RenderCommandBuffer& commands);

private:
  IDirect3DDevice9* m_device;
  D3DCAPS9 m_deviceCaps;
  HandlePool<Mesh, MeshHandle> m_meshes;
  HandlePool<Texture, TextureHandle> m_textures;
  RenderCommandBuffer m_commandBuffer;
  D3DCOLOR m_clearColor;

public:
  static auto Create(HWND window) -> Renderer*;
};

} // namespace basalt::gfx::backend::d3d9

#endif // !BS_GFX_BACKEND_D3D9_RENDERER_H
