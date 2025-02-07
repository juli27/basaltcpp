#pragma once

#include <basalt/api/gfx/backend/adapter.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/base/types.h>

namespace basalt::gfx {

class Factory {
public:
  Factory(Factory const&) = delete;
  Factory(Factory&&) = delete;

  virtual ~Factory() noexcept = default;

  auto operator=(Factory const&) -> Factory& = delete;
  auto operator=(Factory&&) -> Factory& = delete;

  [[nodiscard]]
  auto enumerate_adapters() const -> AdapterInfos;

  [[nodiscard]]
  virtual auto adapter_count() const -> u32 = 0;

  [[nodiscard]]
  virtual auto
  get_adapter_identifier(u32 adapterIndex) const -> AdapterIdentifier = 0;

  [[nodiscard]]
  virtual auto
  get_adapter_device_caps(u32 adapterIndex) const -> DeviceCaps = 0;

  [[nodiscard]]
  virtual auto get_adapter_shared_mode_info(u32 adapterIndex) const
    -> AdapterSharedModeInfo = 0;

  [[nodiscard]]
  virtual auto enum_adapter_exclusive_mode_infos(u32 adapterIndex) const
    -> AdapterExclusiveModeInfos = 0;

protected:
  Factory() = default;
};

} // namespace basalt::gfx
