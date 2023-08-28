#pragma once

#include <basalt/win32/shared/win32_gfx_factory.h>

#include <basalt/gfx/backend/d3d9/d3d9_custom.h>
#include <basalt/gfx/backend/d3d9/types.h>

#include <basalt/api/gfx/types.h>

namespace basalt::gfx {

class D3D9Factory final : public Win32GfxFactory {
public:
  // returns null on failure
  static auto create() -> D3D9FactoryPtr;

  D3D9Factory(const D3D9Factory&) = delete;
  D3D9Factory(D3D9Factory&&) = delete;

  ~D3D9Factory() noexcept override = default;

  auto operator=(const D3D9Factory&) -> D3D9Factory& = delete;
  auto operator=(D3D9Factory&&) -> D3D9Factory& = delete;

  [[nodiscard]] auto get_adapter_monitor(Adapter) const -> HMONITOR;

  [[nodiscard]] auto adapters() const -> const AdapterList& override;

  // don't use. Use create function instead
  D3D9Factory(IDirect3D9Ptr, AdapterList);

private:
  IDirect3D9Ptr mInstance;
  AdapterList mAdapters;

  auto do_create_device_and_swap_chain(HWND,
                                       const DeviceAndSwapChainDesc&) const
    -> DeviceAndSwapChain override;
};

} // namespace basalt::gfx
