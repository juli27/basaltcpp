#pragma once

#include <sandbox/test_case.h>

#include <api/gfx/backend/device.h>
#include <api/gfx/scene_view.h>
#include <api/scene/scene.h>

#include <memory>

namespace d3d9 {

struct Matrices final : TestCase {
  explicit Matrices(basalt::gfx::Device&);

  Matrices(const Matrices&) = delete;
  Matrices(Matrices&&) = delete;

  ~Matrices() override = default;

  auto operator=(const Matrices&) -> Matrices& = delete;
  auto operator=(Matrices &&) -> Matrices& = delete;

  void on_update(const basalt::UpdateContext&) override;
  auto name() -> std::string_view override;

private:
  std::shared_ptr<basalt::Scene> mScene = std::make_shared<basalt::Scene>();
  std::shared_ptr<basalt::gfx::SceneView> mSceneView {};
  entt::entity mTriangle {entt::null};
};

} // namespace d3d9
