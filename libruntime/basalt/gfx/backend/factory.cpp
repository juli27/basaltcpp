#include "factory.h"

#include <basalt/api/gfx/backend/adapter.h>

#include <basalt/api/base/log.h>
#include <basalt/api/base/types.h>

#include <utility>

namespace basalt::gfx {

auto Factory::enumerate_adapters() const -> AdapterInfos {
  auto const adapterCount = adapter_count();

  auto adapterInfos = AdapterInfos{};
  adapterInfos.reserve(adapterCount);

  for (auto i = u32{}; i < adapterCount; i++) {
    auto sharedModeInfo = get_adapter_shared_mode_info(i);
    if (sharedModeInfo.backBufferFormats.empty()) {
      BASALT_LOG_INFO("gfx: adapter {} has no back buffer formats", i);

      continue;
    }

    auto adapterInfo = AdapterInfo{
      std::move(sharedModeInfo),
      enum_adapter_exclusive_mode_infos(i),
      get_adapter_identifier(i),
      get_adapter_device_caps(i),
      i,
    };

    adapterInfos.push_back(std::move(adapterInfo));
  }

  return adapterInfos;
}

} // namespace basalt::gfx
