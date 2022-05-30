#include <basalt/api/solid_color_view.h>

#include <basalt/api/gfx/backend/command_list.h>

#include <utility>

namespace basalt {

using gfx::Attachment;
using gfx::Attachments;
using gfx::CommandList;
using gfx::ResourceCache;

SolidColorView::SolidColorView(const Color& color) : mColor {color} {
}

auto SolidColorView::on_draw(const DrawContext& context) -> void {
  CommandList cmdList {};
  cmdList.clear_attachments(Attachments {Attachment::RenderTarget}, mColor,
                            1.0f, 0);

  context.commandLists.push_back(std::move(cmdList));
}

} // namespace basalt
