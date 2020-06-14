#pragma once

#include <sandbox/test_case.h>

#include <runtime/gfx/scene_view.h>
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

  void on_update(const basalt::UpdateContext&) override;
  auto name() -> std::string_view override;

private:
  std::shared_ptr<basalt::Scene> mScene = std::make_shared<basalt::Scene>();
  std::shared_ptr<basalt::gfx::SceneView> mSceneView {};
};

} // namespace d3d9
