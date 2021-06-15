#pragma once

#include <basalt/api/gfx/backend/context.h>

#include <basalt/gfx/backend/d3d9/d3d9_custom.h>
#include <basalt/gfx/backend/d3d9/device.h>

#include <wrl/client.h>

#include <memory>

namespace basalt::gfx {

// the implicit swap chain of the device
struct D3D9Context final : Context {
  explicit D3D9Context(std::shared_ptr<D3D9Device>);

  [[nodiscard]] auto surface_size() const noexcept -> Size2Du16 override;

  void resize(Size2Du16) override;

  [[nodiscard]] auto device() const noexcept -> DevicePtr override;

  void submit(const Composite&) override;
  void present() override;

private:
  std::shared_ptr<D3D9Device> mDevice {};
  Microsoft::WRL::ComPtr<IDirect3DDevice9> mD3D9Device {};
  Microsoft::WRL::ComPtr<IDirect3DSwapChain9> mImplicitSwapChain {};
};

} // namespace basalt::gfx
