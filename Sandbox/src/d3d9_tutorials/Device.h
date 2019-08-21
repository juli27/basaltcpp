#pragma once
#ifndef D3D9_TUTORIALS_DEVICE_H
#define D3D9_TUTORIALS_DEVICE_H

#include "../ITestCase.h"

#include <memory>

#include <Basalt.h>

namespace d3d9_tuts {

struct Device final : ITestCase {
  Device();
  Device(const Device&) = delete;
  Device(Device&&) = delete;
  virtual ~Device() = default;

  auto operator=(const Device&) -> Device& = delete;
  auto operator=(Device&&) -> Device& = delete;

  void OnShow() override;
  void OnHide() override;
  void OnUpdate() override;

private:
  std::shared_ptr<bs::Scene> mScene;
};

} // namespace d3d9_tuts

#endif // D3D9_TUTORIALS_DEVICE_H
