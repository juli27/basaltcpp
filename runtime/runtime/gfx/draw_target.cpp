#include "draw_target.h"

#include <utility>

using std::shared_ptr;
using std::vector;

namespace basalt::gfx {

DrawTarget::DrawTarget(const Size2Du16 size)
  : mSize {size} {
}

auto DrawTarget::visuals() const -> const vector<Visual>& {
  return mVisuals;
}

auto DrawTarget::size() const -> Size2Du16 {
  return mSize;
}

auto DrawTarget::draw(shared_ptr<Drawable> drawable) -> const Visual& {
  return mVisuals.emplace_back(
    std::move(drawable), Rectangle {0, 0, mSize.width(), mSize.height()});
}

} // namespace basalt::gfx
