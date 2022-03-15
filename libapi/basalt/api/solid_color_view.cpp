#include <basalt/api/solid_color_view.h>

#include <basalt/api/gfx/backend/command_list.h>

namespace basalt {

using gfx::Attachment;
using gfx::Attachments;
using gfx::CommandList;
using gfx::ResourceCache;

SolidColorView::SolidColorView(const Color& color) : mColor {color} {
}

auto SolidColorView::on_draw(const DrawContext&) -> CommandList {
  CommandList cmdList {};
  cmdList.clear_attachments(Attachments {Attachment::Color}, mColor, 1.0f, 0);

  return cmdList;
}

} // namespace basalt
