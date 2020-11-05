#pragma once

#include "drawable.h"

#include "api/shared/color.h"

namespace basalt::gfx {

struct SolidColorView final : Drawable {
  explicit SolidColorView(const Color&);

  auto draw(ResourceCache&, Size2Du16 viewport) -> CommandList override;

  [[nodiscard]] auto clear_color() const -> std::optional<Color> override;

private:
  Color mColor;
};

} // namespace basalt::gfx
