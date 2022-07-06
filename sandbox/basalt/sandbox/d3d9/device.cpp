#include <basalt/sandbox/d3d9/device.h>

#include <basalt/api/prelude.h>

#include <basalt/api/gfx/backend/command_list.h>

#include <utility>

using basalt::Engine;
using basalt::gfx::Attachment;
using basalt::gfx::Attachments;
using basalt::gfx::CommandList;

namespace d3d9 {

Device::Device(Engine&) noexcept {
}

auto Device::on_draw(const DrawContext& context) -> void {
  CommandList cmdList {};
  cmdList.clear_attachments(Attachments {Attachment::RenderTarget},
                            Colors::BLUE, 1.0f, 0);

  context.commandLists.push_back(std::move(cmdList));
}

} // namespace d3d9
