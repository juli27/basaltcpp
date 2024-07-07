#include <basalt/gfx/backend/d3d9/swap_chain.h>

#include <basalt/gfx/backend/d3d9/conversions.h>
#include <basalt/gfx/backend/d3d9/device.h>

#include <basalt/gfx/backend/types.h>

#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/log.h>

#include <basalt/api/base/utils.h>

#include <memory>
#include <utility>

namespace basalt::gfx {

auto D3D9SwapChain::create(D3D9DevicePtr device,
                           IDirect3DSwapChain9Ptr swapChain)
  -> D3D9SwapChainPtr {
  return std::make_shared<D3D9SwapChain>(std::move(device),
                                         std::move(swapChain));
}

auto D3D9SwapChain::device() const noexcept -> DevicePtr {
  return mDevice;
}

auto D3D9SwapChain::get_info() const noexcept -> Info {
  auto pp = D3DPRESENT_PARAMETERS{};
  D3D9CHECK(mSwapChain->GetPresentParameters(&pp));

  return Info{{saturated_cast<u16>(pp.BackBufferWidth),
               saturated_cast<u16>(pp.BackBufferHeight)},
              pp.FullScreen_RefreshRateInHz,
              to_image_format(pp.BackBufferFormat),
              to_multi_sample_count(pp.MultiSampleType),
              !pp.Windowed};
}

auto D3D9SwapChain::reset(ResetDesc const& desc) -> void {
  auto pp = D3DPRESENT_PARAMETERS{};
  D3D9CHECK(mSwapChain->GetPresentParameters(&pp));

  pp.BackBufferFormat = to_d3d(desc.renderTargetFormat);
  pp.MultiSampleType = to_d3d(desc.sampleCount);
  pp.MultiSampleQuality = 0;
  pp.Windowed = !desc.exclusive;

  if (desc.exclusive) {
    auto displayMode = D3DDISPLAYMODE{};
    D3D9CHECK(mSwapChain->GetDisplayMode(&displayMode));

    if (desc.exclusiveDisplayMode.width != 0) {
      pp.BackBufferWidth = desc.exclusiveDisplayMode.width;
      pp.BackBufferHeight = desc.exclusiveDisplayMode.height;
      pp.FullScreen_RefreshRateInHz = desc.exclusiveDisplayMode.refreshRate;
    } else {
      pp.BackBufferWidth = displayMode.Width;
      pp.BackBufferHeight = displayMode.Height;
      pp.FullScreen_RefreshRateInHz = displayMode.RefreshRate;
    }

    if (pp.BackBufferFormat == D3DFMT_UNKNOWN) {
      pp.BackBufferFormat = displayMode.Format;
    }
  } else {
    pp.BackBufferWidth = desc.windowBackBufferSize.width();
    pp.BackBufferHeight = desc.windowBackBufferSize.height();
    pp.FullScreen_RefreshRateInHz = 0;
  }

  mDevice->reset(pp);
}

auto D3D9SwapChain::present() -> PresentResult {
  if (auto const hr =
        mSwapChain->Present(nullptr, nullptr, nullptr, nullptr, 0ul);
      FAILED(hr)) {
    if (hr == D3DERR_DEVICELOST) {
      return PresentResult::DeviceLost;
    }

    BASALT_CRASH("present failed");
  }

  return PresentResult::Ok;
}

D3D9SwapChain::D3D9SwapChain(D3D9DevicePtr device,
                             IDirect3DSwapChain9Ptr swapChain)
  : mDevice{std::move(device)}
  , mSwapChain{std::move(swapChain)} {
  BASALT_ASSERT(mDevice);
  BASALT_ASSERT(mSwapChain);
}

} // namespace basalt::gfx
