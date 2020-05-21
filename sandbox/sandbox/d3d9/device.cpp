#include "sandbox/d3d9/device.h"

#include <runtime/Engine.h>
#include <runtime/Prelude.h>

using namespace std::string_view_literals;

namespace d3d9 {

Device::Device() {
  mScene->set_background_color(Color {0.0f, 0.0f, 1.0f});
}

void Device::on_show() {
  set_current_scene(mScene);
}

void Device::on_hide() {
}

void Device::on_update(const f64) {
}

auto Device::name() -> std::string_view {
  return "Tutorial 1: Creating a Device"sv;
}

} // namespace d3d9
