#pragma once

#include <basalt/sandbox/d3d9/utils.h>

#include <basalt/sandbox/test_case.h>

#include <basalt/api/gfx/scene_view.h>
#include <basalt/api/scene/scene.h>

#include <entt/entity/handle.hpp>

#include <memory>

namespace d3d9 {

struct TexturesTci final : TestCase {
  explicit TexturesTci(basalt::Engine&);

  auto drawable() -> basalt::gfx::DrawablePtr override;

private:
  std::shared_ptr<basalt::Scene> mScene = std::make_shared<basalt::Scene>();
  std::shared_ptr<basalt::gfx::SceneView> mSceneView =
    std::make_shared<basalt::gfx::SceneView>(mScene, create_default_camera());
  entt::handle mCylinder = mScene->create_entity();

  void on_tick(basalt::Engine&) override;
};

} // namespace d3d9
