#include <basalt/sandbox/d3d9/device.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/draw_target.h>
#include <basalt/api/gfx/solid_color_view.h>

#include <memory>

using namespace std::literals;

using std::string_view;

using basalt::gfx::SolidColorView;

namespace d3d9 {

Device::Device() : mDrawable {std::make_shared<SolidColorView>(Colors::BLUE)} {
}

void Device::on_update(const basalt::UpdateContext& ctx) {
  ctx.drawTarget.draw(mDrawable);
}

auto Device::name() -> string_view {
  return "Tutorial 1: Creating a Device"sv;
}

} // namespace d3d9
