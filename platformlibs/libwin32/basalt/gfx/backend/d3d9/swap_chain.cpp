#include "swap_chain.h"

#include "conversions.h"
#include "device.h"

#include <basalt/gfx/backend/types.h>

#include <basalt/api/base/asserts.h>
#include <basalt/api/base/functional.h>

#include <memory>
#include <optional>
#include <utility>
#include <variant>

namespace basalt::gfx {

using std::nullopt;
using std::optional;

auto D3D9SwapChain::create(
  D3D9DevicePtr device, IDirect3DSwapChain9Ptr swapChain) -> D3D9SwapChainPtr {
  return std::make_shared<D3D9SwapChain>(std::move(device),
                                         std::move(swapChain));
}

auto D3D9SwapChain::device() const noexcept -> DevicePtr {
  return mDevice;
}

auto D3D9SwapChain::get_info() const noexcept -> Info {
  auto pp = D3DPRESENT_PARAMETERS{};
  D3D9CHECK(mSwapChain->GetPresentParameters(&pp));

  auto modeInfo = pp.Windowed
                    ? ModeInfo{SharedModeInfo{
                        Size2Du16{static_cast<u16>(pp.BackBufferWidth),
                                  static_cast<u16>(pp.BackBufferHeight)}}}
                    : ModeInfo{ExclusiveModeInfo{
                        DisplayMode{pp.BackBufferWidth, pp.BackBufferHeight,
                                    pp.FullScreen_RefreshRateInHz}}};

  return Info{
    modeInfo,
    to_image_format(pp.BackBufferFormat).value(),
    pp.EnableAutoDepthStencil ? to_image_format(pp.AutoDepthStencilFormat)
                              : nullopt,
    to_multi_sample_count(pp.MultiSampleType),
  };
}

auto D3D9SwapChain::reset(Info const& info) -> void {
  auto prevPp = D3DPRESENT_PARAMETERS{};
  D3D9CHECK(mSwapChain->GetPresentParameters(&prevPp));

  auto pp = to_present_parameters(info);
  pp.SwapEffect = prevPp.SwapEffect;
  pp.PresentationInterval = prevPp.PresentationInterval;
  pp.hDeviceWindow = prevPp.hDeviceWindow;

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

auto D3D9SwapChain::to_present_parameters(Info const& info)
  -> D3DPRESENT_PARAMETERS {
  auto pp = D3DPRESENT_PARAMETERS{};
  pp.BackBufferCount = 1;
  pp.BackBufferFormat = to_d3d(info.colorFormat);

  auto const depthStencilFormat =
    info.depthStencilFormat ? to_d3d(*info.depthStencilFormat) : D3DFMT_UNKNOWN;
  pp.EnableAutoDepthStencil = depthStencilFormat != D3DFMT_UNKNOWN;
  pp.AutoDepthStencilFormat = depthStencilFormat;

  pp.MultiSampleType = to_d3d(info.sampleCount);
  pp.MultiSampleQuality = 0;

  pp.Flags =
    pp.EnableAutoDepthStencil ? D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL : 0;

  std::visit(Overloaded{
               [&](SwapChain::SharedModeInfo const& info) {
                 pp.BackBufferWidth = info.size.width();
                 pp.BackBufferHeight = info.size.height();
                 pp.Windowed = TRUE;
               },
               [&](SwapChain::ExclusiveModeInfo const& info) {
                 pp.BackBufferWidth = info.displayMode.width;
                 pp.BackBufferHeight = info.displayMode.height;
                 pp.Windowed = FALSE;
                 pp.FullScreen_RefreshRateInHz = info.displayMode.refreshRate;
               },
             },
             info.modeInfo);

  return pp;
}

D3D9SwapChain::D3D9SwapChain(D3D9DevicePtr device,
                             IDirect3DSwapChain9Ptr swapChain)
  : mDevice{std::move(device)}
  , mSwapChain{std::move(swapChain)} {
  BASALT_ASSERT(mDevice);
  BASALT_ASSERT(mSwapChain);
}

} // namespace basalt::gfx
