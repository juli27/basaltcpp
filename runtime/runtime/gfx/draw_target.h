#pragma once

#include "runtime/shared/size2d.h"

#include <memory>
#include <vector>

namespace basalt::gfx {

struct Drawable;

using DrawablePtr = std::shared_ptr<Drawable>;

struct DrawTarget final {
  explicit DrawTarget(Size2Du16) noexcept;

  DrawTarget(const DrawTarget&) = delete;
  DrawTarget(DrawTarget&&) = default;

  ~DrawTarget() noexcept = default;

  auto operator=(const DrawTarget&) -> DrawTarget& = delete;
  auto operator=(DrawTarget &&) -> DrawTarget& = default;

  [[nodiscard]] auto drawables() const noexcept
    -> const std::vector<DrawablePtr>&;

  [[nodiscard]] auto size() const noexcept -> Size2Du16;

  void draw(DrawablePtr);

private:
  std::vector<DrawablePtr> mDrawables {};
  Size2Du16 mSize;
};

} // namespace basalt::gfx
