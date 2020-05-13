#include "sandbox/d3d9_tutorials/Device.h"

#include <runtime/Engine.h>
#include <runtime/Prelude.h>

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
  mScene->display_debug_gui();
}

} // namespace d3d9
