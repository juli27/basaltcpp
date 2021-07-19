#include <basalt/api/gfx/surface.h>

#include <utility>

using std::vector;

namespace basalt::gfx {

Surface::Surface(const Size2Du16 size) noexcept : mSize {size} {
}

auto Surface::drawables() const noexcept -> const vector<DrawablePtr>& {
  return mDrawables;
}

auto Surface::size() const noexcept -> Size2Du16 {
  return mSize;
}

void Surface::draw(DrawablePtr drawable) {
  mDrawables.emplace_back(std::move(drawable));
}

} // namespace basalt::gfx
