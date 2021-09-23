#include <basalt/sandbox/d3d9/device.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/solid_color_view.h>

#include <memory>

using namespace std::literals;

using std::string_view;

using basalt::Engine;
using basalt::InputEvent;
using basalt::InputEventHandled;
using basalt::gfx::DrawablePtr;
using basalt::gfx::SolidColorView;

namespace d3d9 {

Device::Device() : mDrawable {std::make_shared<SolidColorView>(Colors::BLUE)} {
}

auto Device::name() -> string_view {
  return "Tutorial 1: Creating a Device"sv;
}

auto Device::drawable() -> DrawablePtr {
  return mDrawable;
}

void Device::tick(Engine&) {
}

auto Device::do_handle_input(const InputEvent&) -> InputEventHandled {
  return InputEventHandled::No;
}

} // namespace d3d9
