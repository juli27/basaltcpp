#pragma once

#include <basalt/sandbox/test_case.h>

#include <basalt/api/gfx/scene_view.h>
#include <basalt/api/scene/scene.h>

#include <entt/entity/handle.hpp>

#include <memory>

namespace d3d9 {

struct Lights final : TestCase {
  explicit Lights(basalt::Engine&);

  auto name() -> std::string_view override;
  auto drawable() -> basalt::gfx::DrawablePtr override;

  void on_update(basalt::Engine&) override;

private:
  std::shared_ptr<basalt::Scene> mScene {std::make_shared<basalt::Scene>()};
  std::shared_ptr<basalt::gfx::SceneView> mSceneView;
  basalt::f32 mLightAngle {0.0f};
  entt::handle mCylinder {mScene->create_entity()};
};

} // namespace d3d9
