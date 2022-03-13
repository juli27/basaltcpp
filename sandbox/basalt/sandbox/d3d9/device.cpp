#include <basalt/sandbox/d3d9/device.h>

#include <basalt/api/prelude.h>

#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/shared/size2d.h>

#include <utility>

using std::tuple;

using basalt::Engine;
using basalt::RectangleU16;
using basalt::Size2Du16;
using basalt::gfx::Attachment;
using basalt::gfx::Attachments;
using basalt::gfx::CommandList;
using basalt::gfx::ResourceCache;

namespace d3d9 {

auto Device::on_draw(ResourceCache&, const Size2Du16 viewport,
                     const RectangleU16&) -> tuple<CommandList, RectangleU16> {
  CommandList cmdList {};
  cmdList.clear_attachments(Attachments {Attachment::Color}, Colors::BLUE, 1.0f,
                            0);

  return tuple {std::move(cmdList), viewport.to_rectangle()};
}

void Device::on_tick(Engine&) {
}

} // namespace d3d9
