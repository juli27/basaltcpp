#pragma once

#include <basalt/win32/shared/win32_gfx_factory.h>

#include "d3d9_custom.h"
#include "types.h"

#include <optional>
#include <vector>

namespace basalt::gfx {

class D3D9Factory final : public Win32GfxFactory {
public:
  static auto create() -> std::optional<D3D9FactoryPtr>;

  // don't use. Use create function instead
  D3D9Factory(IDirect3D9Ptr, std::vector<UINT> suitableAdapters);

  [[nodiscard]]
  auto adapter_count() const -> u32 override;

  [[nodiscard]]
  auto
  get_adapter_identifier(u32 adapterIndex) const -> AdapterIdentifier override;

  [[nodiscard]]
  auto get_adapter_device_caps(u32 adapterIndex) const -> DeviceCaps override;

  [[nodiscard]]
  auto get_adapter_shared_mode_info(u32 adapterIndex) const
    -> AdapterSharedModeInfo override;

  [[nodiscard]]
  auto enum_adapter_exclusive_mode_infos(u32 adapterIndex) const
    -> AdapterExclusiveModeInfos override;

private:
  IDirect3D9Ptr mInstance;
  std::vector<UINT> mSuitableAdapters;

  auto do_create_device_and_swap_chain(
    HWND,
    DeviceAndSwapChainCreateInfo const&) const -> DeviceAndSwapChain override;
};

} // namespace basalt::gfx
