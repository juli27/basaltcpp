#include <basalt/sandbox/d3d9/device.h>

#include <basalt/api/prelude.h>

#include <basalt/api/gfx/backend/command_list.h>

using basalt::gfx::Attachment;
using basalt::gfx::Attachments;
using basalt::gfx::CommandList;

namespace d3d9 {

auto Device::on_draw(const DrawContext&) -> CommandList {
  CommandList cmdList {};
  cmdList.clear_attachments(Attachments {Attachment::Color}, Colors::BLUE, 1.0f,
                            0);

  return cmdList;
}

} // namespace d3d9
