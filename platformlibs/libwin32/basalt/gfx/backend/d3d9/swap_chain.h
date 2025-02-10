#pragma once

#include <basalt/gfx/backend/swap_chain.h>

#include "d3d9_custom.h"
#include "types.h"

namespace basalt::gfx {

class D3D9SwapChain final : public SwapChain {
public:
  static auto create(D3D9DevicePtr, IDirect3DSwapChain9Ptr) -> D3D9SwapChainPtr;

  // don't use. use create() function instead
  D3D9SwapChain(D3D9DevicePtr, IDirect3DSwapChain9Ptr);

  [[nodiscard]]
  auto device() const noexcept -> DevicePtr override;

  [[nodiscard]]
  auto get_info() const noexcept -> Info override;

  auto reset(Info const&) -> void override;

  auto present() -> PresentResult override;

  static auto to_present_parameters(Info const&) -> D3DPRESENT_PARAMETERS;

private:
  D3D9DevicePtr mDevice;
  IDirect3DSwapChain9Ptr mSwapChain;
};

} // namespace basalt::gfx
