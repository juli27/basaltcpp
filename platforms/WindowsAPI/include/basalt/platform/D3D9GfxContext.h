#pragma once
#ifndef BS_D3D9_GFX_CONTEXT_H
#define BS_D3D9_GFX_CONTEXT_H

#include <basalt/platform/IGfxContext.h>

#include <basalt/gfx/backend/d3d9/D3D9Header.h>

namespace basalt::platform::winapi {

class D3D9GfxContext final : public IGfxContext {
public:
  D3D9GfxContext(HWND window);
  D3D9GfxContext(const D3D9GfxContext&) = delete;
  D3D9GfxContext(D3D9GfxContext&&) = delete;
  ~D3D9GfxContext();

  auto operator=(const D3D9GfxContext&) -> D3D9GfxContext& = delete;
  auto operator=(D3D9GfxContext&&) -> D3D9GfxContext& = delete;

  auto create_renderer() -> gfx::backend::IRenderer* override;
  void present() override;

private:
  IDirect3D9* mD3D9Object = nullptr;
  IDirect3DDevice9* mDevice = nullptr;
  D3DPRESENT_PARAMETERS mPresentParams = {};
};

} // basalt::platform::winapi

#endif // BS_D3D9_GFX_CONTEXT_H
