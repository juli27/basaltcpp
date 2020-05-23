#include "sandbox/d3d9/device.h"

#include <runtime/Engine.h>
#include <runtime/prelude.h>

using std::string_view;
using namespace std::literals;

namespace d3d9 {

Device::Device() {
  mScene->set_background_color(Color {0.0f, 0.0f, 1.0f});
}

void Device::on_show() {
  basalt::set_view({mScene});
}

void Device::on_hide() {
}

void Device::on_update(const f64) {
}

auto Device::name() -> string_view {
  return "Tutorial 1: Creating a Device"sv;
}

} // namespace d3d9
