#include "draw_target.h"

#include <utility>

using std::vector;

namespace basalt::gfx {

DrawTarget::DrawTarget(const Size2Du16 size) noexcept : mSize {size} {
}

auto DrawTarget::drawables() const noexcept -> const vector<DrawablePtr>& {
  return mDrawables;
}

auto DrawTarget::size() const noexcept -> Size2Du16 {
  return mSize;
}

void DrawTarget::draw(DrawablePtr drawable) {
  mDrawables.emplace_back(std::move(drawable));
}

} // namespace basalt::gfx
