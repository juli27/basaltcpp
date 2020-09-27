#include "composite.h"

#include <utility>

using std::vector;

namespace basalt::gfx {

Composite::Composite(const Color& background)
  : mBackground {background} {
}

auto Composite::background() const -> const Color& {
  return mBackground;
}

auto Composite::parts() const -> const vector<CommandList>& {
  return mParts;
}

void Composite::add_part(CommandList commandList) {
  mParts.push_back(std::move(commandList));
}

} // namespace basalt::gfx
