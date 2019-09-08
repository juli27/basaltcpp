#include "Device.h"

#include <memory> // make_shared

#include <BasaltPrelude.h> // SetCurrentScene

using bs::Color;
using bs::Scene;

namespace d3d9_tuts {

Device::Device() : mScene(std::make_shared<Scene>()) {
  mScene->set_background_color(Color(0, 0, 255));
}

void Device::OnShow() {
  bs::set_current_scene(mScene);
}

void Device::OnHide() {}

void Device::OnUpdate() {}

} // namespace d3d9_tuts
