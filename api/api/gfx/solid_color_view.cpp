#include "solid_color_view.h"

#include "backend/command_list.h"

#include "api/shared/size2d.h"

namespace basalt::gfx {

SolidColorView::SolidColorView(const Color& color) : mColor {color} {
}

auto SolidColorView::draw(ResourceCache&, Size2Du16) -> CommandList {
  return CommandList {};
}

auto SolidColorView::clear_color() const -> std::optional<Color> {
  return mColor;
}

} // namespace basalt::gfx
