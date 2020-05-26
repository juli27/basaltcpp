#include "context.h"

#include "util.h"

#include "runtime/shared/Log.h"

using std::unique_ptr;

namespace basalt::gfx::backend {

D3D9GfxContext::D3D9GfxContext(
  Microsoft::WRL::ComPtr<IDirect3DDevice9> device
, const D3DPRESENT_PARAMETERS& pp)
  : mDevice {std::move(device)}
  , mRenderer {std::make_unique<D3D9Renderer>(mDevice)}
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
  if (auto hr = mDevice->Present(nullptr, nullptr, nullptr, nullptr);
    FAILED(hr)) {
    if (hr == D3DERR_DEVICELOST) {
      mRenderer->before_reset();

      // TODO: get rid of busy wait
      do {
        hr = mDevice->TestCooperativeLevel();
      } while (hr == D3DERR_DEVICELOST);

      BASALT_ASSERT(hr == D3DERR_DEVICENOTRESET);

      D3D9CALL(mDevice->Reset(&mPresentParams));

      mRenderer->after_reset();

      BASALT_LOG_INFO("d3d9 device reset");
    } else {
      BASALT_ASSERT_MSG(false, "present failed");
    }
  }
}

} // basalt::gfx::backend
