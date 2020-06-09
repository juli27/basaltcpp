#pragma once

#include "drawable.h"

#include <runtime/shared/Types.h>

#include <memory>

namespace basalt::gfx {

struct Rectangle final {
  u16 x {};
  u16 y {};
  u16 width {};
  u16 height {};
};

struct Visual final {
  Visual() = delete;
  Visual(std::shared_ptr<Drawable>, Rectangle);

  Visual(const Visual&) = delete;
  Visual(Visual&&) = default;

  ~Visual() = default;

  auto operator=(const Visual&) -> Visual& = delete;
  auto operator=(Visual&&) -> Visual& = delete;

  [[nodiscard]]
  auto drawable() const -> Drawable&;

  [[nodiscard]]
  auto bounds() const -> Rectangle;

private:
  std::shared_ptr<Drawable> mDrawable {};
  const Rectangle mBounds {};
};

} // namespace basalt::gfx
