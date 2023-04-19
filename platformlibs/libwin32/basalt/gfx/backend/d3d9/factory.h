#pragma once

#include <basalt/win32/shared/win32_gfx_factory.h>

#include <basalt/gfx/backend/d3d9/d3d9_custom.h>
#include <basalt/gfx/backend/d3d9/types.h>

#include <basalt/api/gfx/types.h>

#include <wrl/client.h>

namespace basalt::gfx {

class D3D9Factory final : public Win32GfxFactory {
  // disallow outside construction while enabling make_unique
  struct Token {};

  using InstancePtr = Microsoft::WRL::ComPtr<IDirect3D9>;

public:
  // returns null on failure
  static auto create() -> D3D9FactoryPtr;

  D3D9Factory(Token, InstancePtr, AdapterList);

  D3D9Factory(const D3D9Factory&) = delete;
  D3D9Factory(D3D9Factory&&) = delete;

  ~D3D9Factory() noexcept override = default;

  auto operator=(const D3D9Factory&) -> D3D9Factory& = delete;
  auto operator=(D3D9Factory&&) -> D3D9Factory& = delete;

  [[nodiscard]] auto get_adapter_monitor(Adapter) const -> HMONITOR;

  [[nodiscard]] auto adapters() const -> const AdapterList& override;

private:
  InstancePtr mInstance;
  AdapterList mAdapters;

  auto do_create_device_and_context(HWND, const DeviceAndContextDesc&) const
    -> DeviceAndContext override;
};

} // namespace basalt::gfx
