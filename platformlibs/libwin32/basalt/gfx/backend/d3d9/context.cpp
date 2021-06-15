#include <basalt/gfx/backend/d3d9/context.h>

#include <basalt/gfx/backend/d3d9/util.h>

#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/shared/log.h>
#include <basalt/api/shared/size2d.h>

using Microsoft::WRL::ComPtr;

using std::shared_ptr;

namespace basalt::gfx {

D3D9Context::D3D9Context(shared_ptr<D3D9Device> device)
  : mDevice {std::move(device)}, mD3D9Device {mDevice->device()} {
  BASALT_ASSERT(mDevice);
  BASALT_ASSERT(mD3D9Device);

  D3D9CALL(mD3D9Device->GetSwapChain(0, mImplicitSwapChain.GetAddressOf()));
  BASALT_ASSERT(mImplicitSwapChain);
}

auto D3D9Context::surface_size() const noexcept -> Size2Du16 {
  D3DPRESENT_PARAMETERS pp {};
  D3D9CALL(mImplicitSwapChain->GetPresentParameters(&pp));

  return Size2Du16 {static_cast<u16>(pp.BackBufferWidth),
                    static_cast<u16>(pp.BackBufferHeight)};
}

auto D3D9Context::device() const noexcept -> DevicePtr {
  return mDevice;
}

void D3D9Context::submit(const Composite& composite) {
  // TODO: should we make all rendering code dependent
  // on the success of BeginScene? -> Log error and/or throw exception
  mDevice->begin_execution();

  for (const auto& commandList : composite) {
    PIX_BEGIN_EVENT(0, L"command list");

    mDevice->execute(commandList);

    PIX_END_EVENT();
  }

  mDevice->end_execution();
}

void D3D9Context::resize(const Size2Du16 size) {
  D3DPRESENT_PARAMETERS pp {};
  D3D9CALL(mImplicitSwapChain->GetPresentParameters(&pp));

  BASALT_LOG_DEBUG("resizing back buffer from ({},{}) to ({},{})",
                   pp.BackBufferWidth, pp.BackBufferHeight, size.width(),
                   size.height());

  pp.BackBufferWidth = size.width();
  pp.BackBufferHeight = size.height();

  mDevice->reset(pp);
}

void D3D9Context::present() {
  if (auto hr =
        mImplicitSwapChain->Present(nullptr, nullptr, nullptr, nullptr, 0ul);
      FAILED(hr)) {
    if (hr == D3DERR_DEVICELOST) {
      // TODO: get rid of busy wait (and move to D3D9Device?)
      do {
        hr = mD3D9Device->TestCooperativeLevel();
      } while (hr == D3DERR_DEVICELOST);

      BASALT_ASSERT(hr == D3DERR_DEVICENOTRESET);

      D3DPRESENT_PARAMETERS pp {};
      mImplicitSwapChain->GetPresentParameters(&pp);
      mDevice->reset(pp);

      BASALT_LOG_INFO("d3d9 device reset");
    } else {
      BASALT_CRASH("present failed");
    }
  }
}

} // namespace basalt::gfx
