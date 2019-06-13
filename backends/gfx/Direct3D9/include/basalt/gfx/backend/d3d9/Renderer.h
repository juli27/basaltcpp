#pragma once
#ifndef BS_GFX_BACKEND_D3D9_RENDERER_H
#define BS_GFX_BACKEND_D3D9_RENDERER_H

#include <basalt/gfx/backend/IRenderer.h>

#include <vector>

#include "D3D9Header.h"

namespace basalt {
namespace gfx {
namespace backend {
namespace d3d9 {


struct Mesh {
  IDirect3DVertexBuffer9* vertexBuffer;
  DWORD fvf;
  UINT vertexSize;
  D3DPRIMITIVETYPE primType;
  UINT primCount;
  MeshHandle handle;
};


struct Texture {
  IDirect3DTexture9* texture;
  TextureHandle handle;
};


class Renderer final : public IRenderer {
public:

  Renderer(IDirect3DDevice9* device);


  virtual ~Renderer();

  Renderer() = delete;
  Renderer(const Renderer&) = delete;
  Renderer(Renderer&&) = delete;

public:
  virtual MeshHandle AddMesh(
    void* data, i32 numVertices, const VertexLayout& layout,
    PrimitiveType primitiveType
  ) override;
  virtual void RemoveMesh(MeshHandle meshHandle) override;
  virtual TextureHandle AddTexture(std::string_view filePath) override;
  virtual void RemoveTexture(TextureHandle textureHandle) override;
  virtual void Submit(const RenderCommand& command) override;
  virtual void Submit(const RenderCommandBuffer& commands) override;
  virtual void SetViewProj(
    const math::Mat4f32& view, const math::Mat4f32& projection
  ) override;
  virtual void SetLights(const LightSetup& lights) override;
  virtual void SetClearColor(Color color) override;
  virtual void Render() override;
  virtual void Present() override;
  virtual std::string_view GetName() override;

public:
  Renderer& operator=(const Renderer&) = delete;
  Renderer& operator=(Renderer&&) = delete;

private:
  /**
   * \brief Allocates a new mesh slot.
   *
   * \return the allocated mesh slot.
   */
  Mesh& GenerateMeshSlot();

  /**
   * \brief Return a free mesh slot.
   *
   * The returned mesh slot can be a newly allocated or a recycled one.
   *
   * \return the free mesh slot
   */
  Mesh& GetFreeMeshSlot();

  /**
   * \brief Retrieve the mesh slot for the supplied mesh handle.
   */
  Mesh& GetMesh(MeshHandle meshHandle);

  /**
   * \brief Allocates a new texture slot.
   *
   * \return the allocated texture slot.
   */
  Texture& GenerateTextureSlot();

  /**
   * \brief Return a free texture slot.
   *
   * The returned texture slot can be a newly allocated or a recycled one.
   *
   * \return the free texture slot
   */
  Texture& GetFreeTextureSlot();

  /**
   * \brief Retrieve the texture slot for the supplied texture handle.
   */
  Texture& GetTexture(TextureHandle textureHandle);


  void RenderCommands(const RenderCommandBuffer& commands);

private:
  IDirect3DDevice9* m_device;
  D3DCAPS9 m_deviceCaps;
  std::vector<Mesh> m_meshes;
  std::vector<i16> m_freeMeshSlots;
  std::vector<Texture> m_textures;
  std::vector<i16> m_freeTextureSlots;
  std::vector<RenderCommandBuffer> m_commandBuffers;
  D3DCOLOR m_clearColor;
};

} // namespace d3d9
} // namespace backend
} // namespace gfx
} // namespace basalt

#endif // !BS_GFX_BACKEND_D3D9_RENDERER_H
