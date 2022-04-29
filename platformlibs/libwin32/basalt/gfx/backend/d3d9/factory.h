#pragma once

#include <basalt/gfx/backend/d3d9/d3d9_custom.h>
#include <basalt/gfx/backend/d3d9/types.h>

#include <basalt/gfx/backend/types.h>

#include <basalt/api/gfx/types.h>

#include <wrl/client.h>

namespace basalt::gfx {

struct D3D9Factory final {
private:
  // disallow outside construction while enabling make_unique
  struct Token {};

  using InstancePtr = Microsoft::WRL::ComPtr<IDirect3D9>;

public:
  struct DeviceAndContextDesc final {
    Adapter adapter;
    bool exclusive {false};
  };

  // returns null on failure
  static auto create() -> D3D9FactoryPtr;

  D3D9Factory(Token, InstancePtr);

  D3D9Factory(const D3D9Factory&) = delete;
  D3D9Factory(D3D9Factory&&) = delete;

  ~D3D9Factory() noexcept = default;

  auto operator=(const D3D9Factory&) -> D3D9Factory& = delete;
  auto operator=(D3D9Factory&&) -> D3D9Factory& = delete;

  [[nodiscard]] auto info() const -> const Info&;

  [[nodiscard]] auto get_current_adapter_mode(Adapter) const -> AdapterMode;
  [[nodiscard]] auto get_adapter_monitor(Adapter) const -> HMONITOR;

  auto create_device_and_context(HWND window, const DeviceAndContextDesc&) const
    -> ContextPtr;

private:
  InstancePtr mInstance;
  Info mInfo;
};

} // namespace basalt::gfx
