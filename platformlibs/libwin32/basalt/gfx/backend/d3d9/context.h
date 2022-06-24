#pragma once

#include <basalt/gfx/backend/context.h>

#include <basalt/gfx/backend/d3d9/d3d9_custom.h>
#include <basalt/gfx/backend/d3d9/device.h>
#include <basalt/gfx/backend/d3d9/types.h>

#include <wrl/client.h>

namespace basalt::gfx {

// the implicit swap chain of the device
struct D3D9Context final : Context {
  explicit D3D9Context(D3D9DevicePtr);

  [[nodiscard]] auto surface_size() const noexcept -> Size2Du16 override;

  [[nodiscard]] auto get_status() const noexcept -> ContextStatus override;

  auto reset() -> void override;
  auto reset(const ResetDesc&) -> void override;

  [[nodiscard]] auto device() const noexcept -> DevicePtr override;

  void submit(const Composite&) override;
  auto present() -> PresentResult override;

private:
  D3D9DevicePtr mDevice;
  Microsoft::WRL::ComPtr<IDirect3DDevice9> mD3D9Device {};
  Microsoft::WRL::ComPtr<IDirect3DSwapChain9> mImplicitSwapChain {};
};

} // namespace basalt::gfx
