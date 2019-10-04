#pragma once
#ifndef BS_GFX_BACKEND_D3D9_RENDERER_H
#define BS_GFX_BACKEND_D3D9_RENDERER_H

#include <basalt/gfx/backend/IRenderer.h>

#include <basalt/common/HandlePool.h>

#include "D3D9Header.h"

namespace basalt::gfx::backend::d3d9 {

struct Mesh {
  IDirect3DVertexBuffer9* vertexBuffer = nullptr;
  DWORD fvf = 0u;
  UINT vertexSize = 0u;
  D3DPRIMITIVETYPE primType = D3DPT_POINTLIST;
  UINT primCount = 0u;
};

struct Texture {
  IDirect3DTexture9* texture = nullptr;
};

struct Renderer final : IRenderer {
  Renderer() = delete;
  explicit Renderer(IDirect3DDevice9* device, const D3DPRESENT_PARAMETERS& pp);
  Renderer(const Renderer&) = delete;
  Renderer(Renderer&&) = delete;
  ~Renderer();

  auto operator=(const Renderer&) -> Renderer& = delete;
  auto operator=(Renderer&&) -> Renderer& = delete;

  auto add_mesh(
    void* data, i32 numVertices, const VertexLayout& layout,
    PrimitiveType primitiveType
  ) -> MeshHandle override;
  void remove_mesh(MeshHandle meshHandle) override;
  auto add_texture(std::string_view filePath) -> TextureHandle override;
  void remove_texture(TextureHandle textureHandle) override;
  void submit(const RenderCommand& command) override;
  void set_view_proj(
    const math::Mat4f32& view, const math::Mat4f32& projection
  ) override;
  void set_lights(const LightSetup& lights) override;
  void set_clear_color(Color color) override;
  void render() override;
  auto get_name() -> std::string_view override;

  void new_gui_frame() override;

private:
  void render_commands(const RenderCommandBuffer& commands);

  IDirect3DDevice9* mDevice = nullptr;
  D3DCAPS9 mDeviceCaps = {};
  D3DPRESENT_PARAMETERS mPresentParams;
  HandlePool<Mesh, MeshHandle> mMeshes;
  HandlePool<Texture, TextureHandle> mTextures;
  RenderCommandBuffer mCommandBuffer;
  D3DCOLOR mClearColor = D3DCOLOR_XRGB(0, 0, 0);
};

} // namespace basalt::gfx::backend::d3d9

#endif // !BS_GFX_BACKEND_D3D9_RENDERER_H
