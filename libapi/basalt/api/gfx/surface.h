#pragma once

#include <basalt/api/gfx/types.h>

#include <basalt/api/shared/size2d.h>

#include <vector>

namespace basalt::gfx {

struct Surface final {
  explicit Surface(Size2Du16) noexcept;

  Surface(const Surface&) = delete;
  Surface(Surface&&) noexcept = default;

  ~Surface() noexcept = default;

  auto operator=(const Surface&) -> Surface& = delete;
  auto operator=(Surface&&) noexcept -> Surface& = default;

  [[nodiscard]] auto drawables() const noexcept
    -> const std::vector<DrawablePtr>&;

  [[nodiscard]] auto size() const noexcept -> Size2Du16;

  void draw(DrawablePtr);

private:
  std::vector<DrawablePtr> mDrawables;
  Size2Du16 mSize;
};

} // namespace basalt::gfx
