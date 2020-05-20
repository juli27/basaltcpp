#include "runtime/gfx/backend/d3d9/context.h"

#include "runtime/gfx/backend/d3d9/util.h"

#include "runtime/shared/Log.h"

using std::unique_ptr;

namespace basalt::gfx::backend {

D3D9GfxContext::D3D9GfxContext(
  Microsoft::WRL::ComPtr<IDirect3DDevice9> device
, const D3DPRESENT_PARAMETERS& pp)
  : mDevice {std::move(device)}
  , mRenderer {std::make_unique<D3D9Renderer>(mDevice, mPresentParams)}
  , mPresentParams {pp} {
}

auto D3D9GfxContext::renderer() const -> const std::unique_ptr<D3D9Renderer>& {
  return mRenderer;
}

void D3D9GfxContext::resize(const Size2Du16 size) {
  mRenderer->before_reset();

  mPresentParams.BackBufferWidth = size.width();
  mPresentParams.BackBufferHeight = size.height();
  D3D9CALL(mDevice->Reset(&mPresentParams));

  mRenderer->after_reset();

  BASALT_LOG_DEBUG("resized d3d9 back buffer");
}

void D3D9GfxContext::present() {
  mDevice->Present(nullptr, nullptr, nullptr, nullptr);
}

} // basalt::gfx::backend
