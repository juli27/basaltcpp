#include "Device.h"

#include <memory>

#include <Basalt.h>


d3d9_tuts::Device::Device() : mScene(std::make_shared<bs::Scene>()) {
  mScene->SetBackgroundColor(bs::Color(0, 0, 255));
}

void d3d9_tuts::Device::OnShow() {
  bs::SetCurrentScene(mScene);
}

void d3d9_tuts::Device::OnHide() {}

void d3d9_tuts::Device::OnUpdate() {}
