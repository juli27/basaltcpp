#pragma once

#include <sandbox/test_case.h>

#include <api/gfx/scene_view.h>
#include <api/scene/scene.h>

#include <entt/entity/handle.hpp>

#include <memory>

namespace d3d9 {

struct Matrices final : TestCase {
  explicit Matrices(basalt::Engine&);

  Matrices(const Matrices&) = delete;
  Matrices(Matrices&&) = delete;

  ~Matrices() override = default;

  auto operator=(const Matrices&) -> Matrices& = delete;
  auto operator=(Matrices &&) -> Matrices& = delete;

  void on_update(const basalt::UpdateContext&) override;
  auto name() -> std::string_view override;

private:
  std::shared_ptr<basalt::Scene> mScene = std::make_shared<basalt::Scene>();
  std::shared_ptr<basalt::gfx::SceneView> mSceneView;
  entt::handle mTriangle {mScene->create_entity()};
};

} // namespace d3d9
