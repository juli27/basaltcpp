#pragma once

#include <basalt/api/view.h>

#include <basalt/api/shared/color.h>

namespace basalt {

struct SolidColorView final : View {
  explicit SolidColorView(const Color&);

private:
  Color mColor;

  auto on_draw(gfx::ResourceCache&, Size2Du16 viewport)
    -> gfx::CommandList override;
};

} // namespace basalt
