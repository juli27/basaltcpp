#include "sandbox/d3d9/device.h"

#include <runtime/prelude.h>

using namespace std::literals;

using std::string_view;

using basalt::gfx::View;

namespace d3d9 {

Device::Device() {
  mScene->set_background_color(Colors::BLUE);
}

auto Device::view(const basalt::Size2Du16) -> View {
  return View {mScene};
}

void Device::on_update(const f64) {
}

auto Device::name() -> string_view {
  return "Tutorial 1: Creating a Device"sv;
}

} // namespace d3d9
