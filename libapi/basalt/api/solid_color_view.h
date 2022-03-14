#pragma once

#include <basalt/api/gfx/drawable.h>

#include <basalt/api/shared/color.h>

namespace basalt {

struct SolidColorView final : gfx::Drawable {
  explicit SolidColorView(const Color&);

private:
  Color mColor;

  auto on_draw(gfx::ResourceCache&, Size2Du16 viewport,
               const RectangleU16& clip)
    -> std::tuple<gfx::CommandList, RectangleU16> override;
};

} // namespace basalt
