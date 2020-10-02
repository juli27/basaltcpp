#pragma once

#include <sandbox/test_case.h>

#include <runtime/gfx/scene_view.h>
#include <runtime/scene/scene.h>

#include <memory>

namespace d3d9 {

struct Meshes final : TestCase {
  Meshes();

  Meshes(const Meshes&) = delete;
  Meshes(Meshes&&) = delete;

  ~Meshes() override = default;

  auto operator=(const Meshes&) -> Meshes& = delete;
  auto operator=(Meshes &&) -> Meshes& = delete;

  void on_update(const basalt::UpdateContext&) override;
  auto name() -> std::string_view override;

private:
  std::shared_ptr<basalt::Scene> mScene = std::make_shared<basalt::Scene>();
  std::shared_ptr<basalt::gfx::SceneView> mSceneView {};
  entt::entity mTiger {entt::null};
};

} // namespace d3d9
