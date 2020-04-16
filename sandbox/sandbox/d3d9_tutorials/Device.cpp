#include "Device.h"

#include <runtime/Prelude.h>

#include <runtime/Engine.h>

namespace d3d9_tuts {

Device::Device() {
  mScene->set_background_color(Color { 0.0f, 0.0f, 1.0f });
}

void Device::on_show() {
  set_current_scene(mScene);
}

void Device::on_hide() {}

void Device::on_update() {}

} // namespace d3d9_tuts
