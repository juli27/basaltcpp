#include <basalt/api/gfx/solid_color_view.h>

#include <basalt/api/gfx/command_list_recorder.h>

#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/shared/size2d.h>

namespace basalt::gfx {

SolidColorView::SolidColorView(const Color& color) : mColor {color} {
}

auto SolidColorView::draw(ResourceCache&, const Size2Du16 viewport,
                          const RectangleU16&)
  -> std::tuple<CommandList, RectangleU16> {
  CommandListRecorder cmdListRecorder;
  cmdListRecorder.clear(mColor);

  return {cmdListRecorder.take_cmd_list(), viewport.to_rectangle()};
}

} // namespace basalt::gfx
