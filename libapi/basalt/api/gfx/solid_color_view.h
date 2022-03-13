#pragma once

#include <basalt/api/gfx/drawable.h>

#include <basalt/api/shared/color.h>

namespace basalt::gfx {

struct SolidColorView final : Drawable {
  explicit SolidColorView(const Color&);

private:
  Color mColor;

  auto on_draw(ResourceCache&, Size2Du16 viewport, const RectangleU16& clip)
    -> std::tuple<CommandList, RectangleU16> override;
};

} // namespace basalt::gfx
