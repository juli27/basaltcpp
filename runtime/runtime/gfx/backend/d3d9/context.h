#pragma once
#ifndef BASALT_RUNTIME_GFX_BACKEND_D3D9_CONTEXT_H
#define BASALT_RUNTIME_GFX_BACKEND_D3D9_CONTEXT_H

#include "runtime/gfx/backend/context.h"

#include "runtime/gfx/backend/d3d9/d3d9_custom.h"
#include "runtime/gfx/backend/d3d9/renderer.h"

#include "runtime/shared/Size2D.h"

#include <wrl/client.h>

#include <memory>

namespace basalt::gfx::backend {

struct D3D9GfxContext final : IGfxContext {
  D3D9GfxContext(
    Microsoft::WRL::ComPtr<IDirect3DDevice9> device
  , const D3DPRESENT_PARAMETERS& pp);

  D3D9GfxContext(const D3D9GfxContext&) = delete;
  D3D9GfxContext(D3D9GfxContext&&) = delete;

  ~D3D9GfxContext() override = default;

  auto operator=(const D3D9GfxContext&) -> D3D9GfxContext& = delete;
  auto operator=(D3D9GfxContext&&) -> D3D9GfxContext& = delete;

  [[nodiscard]]
  auto renderer() const -> const std::unique_ptr<D3D9Renderer>&;
  void resize(Size2Du16 size);

  void present() override;

private:
  Microsoft::WRL::ComPtr<IDirect3DDevice9> mDevice {};
  std::unique_ptr<D3D9Renderer> mRenderer {};
  D3DPRESENT_PARAMETERS mPresentParams {};
};

} // basalt::gfx::backend

#endif // BASALT_RUNTIME_GFX_BACKEND_D3D9_CONTEXT_H
