#pragma once

#include <runtime/gfx/backend/context.h>

#include "d3d9_custom.h"
#include "device.h"

#include <wrl/client.h>

#include <memory>

namespace basalt::gfx {

// the implicit swap chain of the device
struct D3D9Context final : Context {
  D3D9Context(std::shared_ptr<D3D9Device>, const D3DPRESENT_PARAMETERS&);

  [[nodiscard]] auto surface_size() const noexcept -> Size2Du16 override;

  void resize(Size2Du16) override;

  [[nodiscard]] auto device() const noexcept -> D3D9Device& override;

  void submit(const Composite&) override;
  void present() override;

private:
  std::shared_ptr<D3D9Device> mDevice {};
  Microsoft::WRL::ComPtr<IDirect3DDevice9> mD3D9Device {};
  D3DPRESENT_PARAMETERS mPresentParams {};
};

} // namespace basalt::gfx
