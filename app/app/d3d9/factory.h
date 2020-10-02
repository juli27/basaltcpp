#pragma once

#include "d3d9_custom.h"
#include "types.h"

#include <wrl/client.h>

#include <memory>
#include <optional>
#include <tuple>

namespace basalt::gfx {

struct D3D9Factory;
using D3D9FactoryPtr = std::unique_ptr<D3D9Factory>;

struct Context;
using ContextPtr = std::shared_ptr<Context>;

struct Device;
using DevicePtr = std::shared_ptr<Device>;

struct D3D9Factory final {
  explicit D3D9Factory(Microsoft::WRL::ComPtr<IDirect3D9> factory);

  D3D9Factory(const D3D9Factory& other) = delete;
  D3D9Factory(D3D9Factory&& other) = delete;

  ~D3D9Factory() noexcept = default;

  auto operator=(const D3D9Factory& other) -> D3D9Factory& = delete;
  auto operator=(D3D9Factory&& other) -> D3D9Factory& = delete;

  [[nodiscard]] auto adapter_info() const noexcept -> const AdapterInfo&;

  auto create_device_and_context(HWND window) const
    -> std::tuple<DevicePtr, ContextPtr>;

private:
  Microsoft::WRL::ComPtr<IDirect3D9> mFactory;
  AdapterInfo mAdapterInfo {};

public:
  static auto create() -> std::optional<D3D9FactoryPtr>;
};

} // namespace basalt::gfx
