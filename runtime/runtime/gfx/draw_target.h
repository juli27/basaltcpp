#pragma once

#include <runtime/shared/size2d.h>

#include <memory>
#include <vector>

namespace basalt::gfx {

struct Drawable;

using DrawablePtr = std::shared_ptr<Drawable>;

struct DrawTarget final {
  DrawTarget() = delete;
  explicit DrawTarget(Size2Du16);

  DrawTarget(const DrawTarget&) = default;
  DrawTarget(DrawTarget&&) = default;

  ~DrawTarget() = default;

  auto operator=(const DrawTarget&) -> DrawTarget& = default;
  auto operator=(DrawTarget&&) -> DrawTarget& = default;

  [[nodiscard]]
  auto drawables() const -> const std::vector<DrawablePtr>&;

  [[nodiscard]]
  auto size() const -> Size2Du16;

  void draw(std::shared_ptr<Drawable>);

private:
  std::vector<DrawablePtr> mDrawables {};
  Size2Du16 mSize;
};

} // namespace basalt::gfx
