#include "visual.h"

#include <runtime/shared/Asserts.h>

#include <utility>

using std::shared_ptr;

namespace basalt::gfx {

Visual::Visual(shared_ptr<Drawable> drawable, const Rectangle rect)
  : mDrawable {std::move(drawable)}, mBounds {rect} {
  BASALT_ASSERT(mDrawable);
}

auto Visual::drawable() const -> Drawable& {
  return *mDrawable;
}

auto Visual::bounds() const -> Rectangle {
  return mBounds;
}

} // namespace basalt::gfx
