#pragma once

#include "visual.h"

#include <runtime/shared/size2d.h>

#include <memory>
#include <vector>

namespace basalt::gfx {

struct Drawable;

struct DrawTarget final {
  DrawTarget() = delete;
  explicit DrawTarget(Size2Du16);

  DrawTarget(const DrawTarget&) = default;
  DrawTarget(DrawTarget&&) = default;

  ~DrawTarget() = default;

  auto operator=(const DrawTarget&) -> DrawTarget& = default;
  auto operator=(DrawTarget&&) -> DrawTarget& = default;

  [[nodiscard]]
  auto visuals() const -> const std::vector<Visual>&;

  [[nodiscard]]
  auto size() const -> Size2Du16;

  auto draw(std::shared_ptr<Drawable>) -> const Visual&;

private:
  std::vector<Visual> mVisuals {};
  Size2Du16 mSize;
};

} // namespace basalt::gfx
