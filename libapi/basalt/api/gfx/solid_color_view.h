#pragma once

#include <basalt/api/gfx/drawable.h>

#include <basalt/api/shared/color.h>

namespace basalt::gfx {

struct SolidColorView final : Drawable {
  explicit SolidColorView(const Color&);

  auto draw(ResourceCache&, Size2Du16 viewport, const RectangleU16& clip)
    -> std::tuple<CommandList, RectangleU16> override;

private:
  Color mColor;
};

} // namespace basalt::gfx
