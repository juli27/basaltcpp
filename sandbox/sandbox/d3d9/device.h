#pragma once
#ifndef SANDBOX_D3D9_DEVICE_H
#define SANDBOX_D3D9_DEVICE_H

#include "sandbox/test_case.h"

#include <runtime/scene/scene.h>

#include <memory>

namespace d3d9 {

struct Device final : TestCase {
  Device();

  Device(const Device&) = delete;
  Device(Device&&) = delete;

  ~Device() override = default;

  auto operator=(const Device&) -> Device& = delete;
  auto operator=(Device&&) -> Device& = delete;

  auto view(basalt::Size2Du16 windowSize) -> basalt::gfx::SceneView override;
  void on_update(basalt::f64 deltaTime) override;
  auto name() -> std::string_view override;

private:
  std::shared_ptr<basalt::Scene> mScene = std::make_shared<basalt::Scene>();
};

} // namespace d3d9

#endif // SANDBOX_D3D9_DEVICE_H
