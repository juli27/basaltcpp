#include "solid_color_view.h"

#include "command_list_recorder.h"

#include "backend/command_list.h"

#include "api/shared/size2d.h"

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
