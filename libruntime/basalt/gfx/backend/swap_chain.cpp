#include "swap_chain.h"

#include "basalt/api/base/functional.h"
#include "basalt/api/base/types.h"

#include <variant>

namespace basalt::gfx {

auto SwapChain::Info::is_exclusive() const -> bool {
  return std::holds_alternative<ExclusiveModeInfo>(modeInfo);
}

auto SwapChain::Info::size() const -> Size2Du16 {
  return std::visit(
    Overloaded{
      [](gfx::SwapChain::SharedModeInfo const& info) { return info.size; },
      [](gfx::SwapChain::ExclusiveModeInfo const& info) {
        return Size2Du16{static_cast<u16>(info.displayMode.width),
                         static_cast<u16>(info.displayMode.height)};
      },
    },
    modeInfo);
}

} // namespace basalt::gfx
