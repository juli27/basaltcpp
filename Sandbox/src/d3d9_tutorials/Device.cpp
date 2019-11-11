#include "Device.h"

#include <basalt/Prelude.h> // SetCurrentScene

#include <memory> // make_shared

using basalt::Color;
using basalt::Scene;

namespace d3d9_tuts {

Device::Device() : mScene(std::make_shared<Scene>()) {
  mScene->set_background_color(Color(0, 0, 255));
}

void Device::on_show() {
  set_current_scene(mScene);
}

void Device::on_hide() {}

void Device::on_update() {}

} // namespace d3d9_tuts
