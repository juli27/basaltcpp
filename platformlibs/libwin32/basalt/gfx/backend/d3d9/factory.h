#pragma once

#include <basalt/gfx/backend/d3d9/d3d9_custom.h>
#include <basalt/gfx/backend/d3d9/types.h>

#include <basalt/gfx/types.h>

#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/base/types.h>

#include <wrl/client.h>

#include <optional>
#include <tuple>

namespace basalt::gfx {

struct D3D9Factory final {
  struct DeviceAndContextDesc final {
    u32 adapterIndex {0};
    bool exclusive {false};
  };

  explicit D3D9Factory(Microsoft::WRL::ComPtr<IDirect3D9> factory);

  D3D9Factory(const D3D9Factory&) = delete;
  D3D9Factory(D3D9Factory&&) = delete;

  ~D3D9Factory() noexcept = default;

  auto operator=(const D3D9Factory&) -> D3D9Factory& = delete;
  auto operator=(D3D9Factory&&) -> D3D9Factory& = delete;

  [[nodiscard]] auto get_adapter_count() const -> u32;
  [[nodiscard]] auto get_current_adapter_mode(u32 adapterIndex) const
    -> AdapterMode;
  [[nodiscard]] auto get_adapter_monitor(u32 adapterIndex) const -> HMONITOR;
  [[nodiscard]] auto query_adapter_info(u32 adapterIndex = 0) const
    -> AdapterInfo;
  [[nodiscard]] auto query_adapter_modes(u32 adapterIndex) const
    -> AdapterModeList;

  auto create_device_and_context(HWND window, const DeviceAndContextDesc&) const
    -> std::tuple<DevicePtr, ContextPtr>;

private:
  Microsoft::WRL::ComPtr<IDirect3D9> mFactory;

public:
  // returns null on failure
  static auto create() -> D3D9FactoryPtr;
};

} // namespace basalt::gfx
