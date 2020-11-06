#include "context.h"

#include "util.h"

#include <api/gfx/backend/command_list.h>

#include <api/shared/log.h>
#include <api/shared/size2d.h>

using std::shared_ptr;

namespace basalt::gfx {

D3D9Context::D3D9Context(shared_ptr<D3D9Device> device,
                         const D3DPRESENT_PARAMETERS& pp)
  : mDevice {std::move(device)}
  , mD3D9Device {mDevice->device()}
  , mPresentParams {pp} {
  BASALT_ASSERT(mDevice);
  BASALT_ASSERT(mD3D9Device);
}

auto D3D9Context::surface_size() const noexcept -> Size2Du16 {
  return Size2Du16 {static_cast<u16>(mPresentParams.BackBufferWidth),
                    static_cast<u16>(mPresentParams.BackBufferHeight)};
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
  BASALT_LOG_DEBUG("resizing back buffer from ({},{}) to ({},{})",
                   mPresentParams.BackBufferWidth,
                   mPresentParams.BackBufferHeight, size.width(),
                   size.height());

  mDevice->before_reset();

  mPresentParams.BackBufferWidth = size.width();
  mPresentParams.BackBufferHeight = size.height();
  D3D9CALL(mD3D9Device->Reset(&mPresentParams));

  mDevice->after_reset();
}

void D3D9Context::present() {
  if (auto hr = mD3D9Device->Present(nullptr, nullptr, nullptr, nullptr);
      FAILED(hr)) {
    if (hr == D3DERR_DEVICELOST) {
      mDevice->before_reset();

      // TODO: get rid of busy wait
      do {
        hr = mD3D9Device->TestCooperativeLevel();
      } while (hr == D3DERR_DEVICELOST);

      BASALT_ASSERT(hr == D3DERR_DEVICENOTRESET);

      D3D9CALL(mD3D9Device->Reset(&mPresentParams));

      mDevice->after_reset();

      BASALT_LOG_INFO("d3d9 device reset");
    } else {
      BASALT_ASSERT_MSG(false, "present failed");
    }
  }
}

} // namespace basalt::gfx
