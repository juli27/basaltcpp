#pragma once

#include <basalt/sandbox/d3d9/utils.h>

#include <basalt/sandbox/test_case.h>

#include <basalt/api/gfx/scene_view.h>
#include <basalt/api/scene/scene.h>

#include <entt/entity/handle.hpp>

#include <memory>

namespace d3d9 {

struct Meshes final : TestCase {
  explicit Meshes(basalt::Engine&);

  Meshes(const Meshes&) = delete;
  Meshes(Meshes&&) = delete;

  ~Meshes() override = default;

  auto operator=(const Meshes&) -> Meshes& = delete;
  auto operator=(Meshes&&) -> Meshes& = delete;

  auto name() -> std::string_view override;
  auto drawable() -> basalt::gfx::DrawablePtr override;

  void on_update(const basalt::UpdateContext&) override;

private:
  std::shared_ptr<basalt::Scene> mScene = std::make_shared<basalt::Scene>();
  std::shared_ptr<basalt::gfx::SceneView> mSceneView =
    std::make_shared<basalt::gfx::SceneView>(mScene, create_default_camera());
  entt::handle mTiger = mScene->create_entity();
};

} // namespace d3d9
