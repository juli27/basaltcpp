#pragma once
#ifndef D3D9_TUTORIALS_DEVICE_H
#define D3D9_TUTORIALS_DEVICE_H

#include "sandbox/TestCase.h"

#include <runtime/Scene.h>

#include <memory>

namespace d3d9_tuts {

struct Device final : TestCase {
  Device();

  Device(const Device&) = delete;
  Device(Device&&) = delete;

  ~Device() override = default;

  auto operator=(const Device&) -> Device& = delete;
  auto operator=(Device&&) -> Device& = delete;

  void on_show() override;
  void on_hide() override;
  void on_update() override;

private:
  std::shared_ptr<basalt::Scene> mScene = std::make_shared<basalt::Scene>();
};

} // namespace d3d9_tuts

#endif // D3D9_TUTORIALS_DEVICE_H