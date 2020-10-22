#include "device.h"

#include <api/engine.h>
#include <api/prelude.h>

#include <api/gfx/drawable.h>
#include <api/gfx/draw_target.h>
#include <api/gfx/backend/command_list.h>

#include <memory>

using namespace std::literals;

using std::string_view;

using basalt::Size2Du16;
using basalt::gfx::CommandList;
using basalt::gfx::Drawable;
using basalt::gfx::ResourceCache;

namespace d3d9 {

namespace {

struct MyDrawable final : Drawable {
  auto draw(ResourceCache&, Size2Du16) -> CommandList override {
    return CommandList {};
  }

  [[nodiscard]] auto clear_color() const -> std::optional<Color> override {
    return Colors::BLUE;
  }
};

} // namespace

Device::Device() : mDrawable {std::make_shared<MyDrawable>()} {
}

void Device::on_update(const basalt::UpdateContext& ctx) {
  ctx.drawTarget.draw(mDrawable);
}

auto Device::name() -> string_view {
  return "Tutorial 1: Creating a Device"sv;
}

} // namespace d3d9
