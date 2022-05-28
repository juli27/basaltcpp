#include <basalt/gfx/backend/d3d9/context.h>

#include <basalt/gfx/backend/d3d9/util.h>

#include <basalt/gfx/backend/types.h>

#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/log.h>

using std::unique_ptr;

namespace basalt::gfx {

namespace {

auto to_context_status(const HRESULT hr) -> ContextStatus {
  if (SUCCEEDED(hr)) {
    return ContextStatus::Ok;
  }

  switch (hr) {
  case D3DERR_DEVICELOST:
    return ContextStatus::DeviceLost;

  case D3DERR_DEVICENOTRESET:
    return ContextStatus::ResetNeeded;

  default:
    return ContextStatus::Error;
  }
}

} // namespace

D3D9Context::D3D9Context(unique_ptr<D3D9Device> device)
  : mDevice {std::move(device)}, mD3D9Device {mDevice->device()} {
  BASALT_ASSERT(mDevice);
  BASALT_ASSERT(mD3D9Device);

  D3D9CHECK(mD3D9Device->GetSwapChain(0, mImplicitSwapChain.GetAddressOf()));
  BASALT_ASSERT(mImplicitSwapChain);
}

auto D3D9Context::surface_size() const noexcept -> Size2Du16 {
  D3DPRESENT_PARAMETERS pp {};
  D3D9CHECK(mImplicitSwapChain->GetPresentParameters(&pp));

  return Size2Du16 {static_cast<u16>(pp.BackBufferWidth),
                    static_cast<u16>(pp.BackBufferHeight)};
}

auto D3D9Context::get_status() const noexcept -> ContextStatus {
  return to_context_status(mD3D9Device->TestCooperativeLevel());
}

auto D3D9Context::reset() -> void {
  D3DPRESENT_PARAMETERS pp {};
  D3D9CHECK(mImplicitSwapChain->GetPresentParameters(&pp));

  mDevice->reset(pp);
}

auto D3D9Context::reset(const ResetDesc& desc) -> void {
  D3DPRESENT_PARAMETERS pp {};
  D3D9CHECK(mImplicitSwapChain->GetPresentParameters(&pp));

  pp.BackBufferFormat = to_d3d(desc.renderTargetFormat);
  pp.MultiSampleType = to_d3d(desc.sampleCount);
  pp.MultiSampleQuality = 0;
  pp.Windowed = !desc.exclusive;

  if (desc.exclusive) {
    pp.BackBufferWidth = desc.exclusiveDisplayMode.width;
    pp.BackBufferHeight = desc.exclusiveDisplayMode.height;
    pp.FullScreen_RefreshRateInHz = desc.exclusiveDisplayMode.refreshRate;
  } else {
    pp.BackBufferWidth = desc.windowBackBufferSize.width();
    pp.BackBufferHeight = desc.windowBackBufferSize.height();
    pp.FullScreen_RefreshRateInHz = 0;
  }

  mDevice->reset(pp);
}

auto D3D9Context::device() const noexcept -> Device& {
  return *mDevice;
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

auto D3D9Context::present() -> PresentResult {
  if (const HRESULT hr =
        mImplicitSwapChain->Present(nullptr, nullptr, nullptr, nullptr, 0ul);
      FAILED(hr)) {
    if (hr == D3DERR_DEVICELOST) {
      return PresentResult::DeviceLost;
    }

    BASALT_CRASH("present failed");
  }

  return PresentResult::Ok;
}

} // namespace basalt::gfx
