#pragma once

#include <basalt/api/view.h>

#include <basalt/sandbox/d3d9/utils.h>

#include <basalt/api/scene_view.h>
#include <basalt/api/types.h>

#include <basalt/api/scene/scene.h>

#include <entt/entity/handle.hpp>

#include <memory>

namespace d3d9 {

struct TexturesTci final : basalt::View {
  explicit TexturesTci(basalt::Engine&);

private:
  std::shared_ptr<basalt::Scene> mScene = std::make_shared<basalt::Scene>();
  basalt::SceneViewPtr mSceneView =
    std::make_shared<basalt::SceneView>(mScene, create_default_camera());
  entt::handle mCylinder = mScene->create_entity();

  void on_tick(basalt::Engine&) override;
};

} // namespace d3d9
