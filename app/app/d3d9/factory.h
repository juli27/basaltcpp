#pragma once

#include "d3d9_custom.h"
#include "types.h"

#include "app/gfx/types.h"

#include <api/gfx/backend/types.h>

#include <api/shared/types.h>

#include <wrl/client.h>

#include <optional>
#include <tuple>

namespace basalt::gfx {

struct D3D9Factory final {
  explicit D3D9Factory(Microsoft::WRL::ComPtr<IDirect3D9> factory);

  D3D9Factory(const D3D9Factory&) = delete;
  D3D9Factory(D3D9Factory&&) = delete;

  ~D3D9Factory() noexcept = default;

  auto operator=(const D3D9Factory&) -> D3D9Factory& = delete;
  auto operator=(D3D9Factory &&) -> D3D9Factory& = delete;

  auto get_current_adapter_mode() const -> AdapterMode;
  [[nodiscard]] auto query_adapter_info() const -> AdapterInfo;

  auto create_device_and_context(HWND window, const Config&) const
    -> std::tuple<DevicePtr, ContextPtr>;

private:
  Microsoft::WRL::ComPtr<IDirect3D9> mFactory;

public:
  // returns null on failure
  static auto create() -> D3D9FactoryPtr;
};

} // namespace basalt::gfx
