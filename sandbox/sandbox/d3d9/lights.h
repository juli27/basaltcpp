#pragma once

#include <sandbox/test_case.h>

#include <runtime/gfx/backend/device.h>
#include <runtime/gfx/scene_view.h>
#include <runtime/scene/scene.h>

#include <memory>

namespace d3d9 {

struct Lights final : TestCase {
  explicit Lights(basalt::gfx::Device&);

  Lights(const Lights&) = delete;
  Lights(Lights&&) = delete;

  ~Lights() override = default;

  auto operator=(const Lights&) -> Lights& = delete;
  auto operator=(Lights &&) -> Lights& = delete;

  void on_update(const basalt::UpdateContext&) override;
  auto name() -> std::string_view override;

private:
  std::shared_ptr<basalt::Scene> mScene {std::make_shared<basalt::Scene>()};
  std::shared_ptr<basalt::gfx::SceneView> mSceneView {};
  basalt::f32 mLightAngle {0.0f};
  entt::entity mCylinder {entt::null};
};

} // namespace d3d9
