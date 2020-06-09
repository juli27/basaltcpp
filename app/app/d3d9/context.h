#pragma once

#include <runtime/gfx/backend/context.h>

#include "d3d9_custom.h"
#include "renderer.h"

#include <wrl/client.h>

#include <memory>

namespace basalt::gfx::backend {

struct D3D9Context final : IGfxContext {
  D3D9Context(
    Microsoft::WRL::ComPtr<IDirect3DDevice9>, const D3DPRESENT_PARAMETERS&);

  D3D9Context(const D3D9Context&) = delete;
  D3D9Context(D3D9Context&&) = delete;

  ~D3D9Context() noexcept override = default;

  auto operator=(const D3D9Context&) -> D3D9Context& = delete;
  auto operator=(D3D9Context&&) -> D3D9Context& = delete;

  [[nodiscard]]
  auto surface_size() const noexcept -> Size2Du16 override;

  [[nodiscard]]
  auto renderer() const noexcept -> D3D9Renderer& override;

  void resize(Size2Du16);
  void present() override;

private:
  Microsoft::WRL::ComPtr<IDirect3DDevice9> mDevice {};
  std::unique_ptr<D3D9Renderer> mRenderer {};
  D3DPRESENT_PARAMETERS mPresentParams {};
};

} // basalt::gfx::backend
