#include <basalt/api/gfx/solid_color_view.h>

#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/shared/size2d.h>

#include <utility>

namespace basalt::gfx {

SolidColorView::SolidColorView(const Color& color) : mColor {color} {
}

auto SolidColorView::draw(ResourceCache&, const Size2Du16 viewport,
                          const RectangleU16&)
  -> std::tuple<CommandList, RectangleU16> {
  CommandList cmdList {};
  cmdList.clear(mColor);

  return {std::move(cmdList), viewport.to_rectangle()};
}

} // namespace basalt::gfx
