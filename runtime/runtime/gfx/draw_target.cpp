#include "draw_target.h"

#include <utility>

using std::shared_ptr;
using std::vector;

namespace basalt::gfx {

DrawTarget::DrawTarget(const Size2Du16 size)
  : mSize {size} {
}

auto DrawTarget::drawables() const -> const vector<DrawablePtr>& {
  return mDrawables;
}

auto DrawTarget::size() const -> Size2Du16 {
  return mSize;
}

void DrawTarget::draw(shared_ptr<Drawable> drawable) {
  mDrawables.push_back(std::move(drawable));
}

} // namespace basalt::gfx
