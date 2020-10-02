#include "composite.h"

#include <utility>

using std::vector;

namespace basalt::gfx {

Composite::Composite(const Color& background) noexcept
  : mBackground {background} {
}

auto Composite::background() const noexcept -> const Color& {
  return mBackground;
}

auto Composite::parts() const noexcept -> const vector<CommandList>& {
  return mParts;
}

void Composite::add_part(CommandList commandList) {
  mParts.emplace_back(std::move(commandList));
}

} // namespace basalt::gfx
