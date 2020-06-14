#pragma once

#include <sandbox/test_case.h>

#include <runtime/gfx/backend/device.h>
#include <runtime/gfx/scene_view.h>
#include <runtime/scene/scene.h>

#include <memory>

namespace d3d9 {

struct Matrices final : TestCase {
  Matrices() = delete;
  explicit Matrices(basalt::gfx::Device&);

  Matrices(const Matrices&) = delete;
  Matrices(Matrices&&) = delete;

  ~Matrices() override = default;

  auto operator=(const Matrices&) -> Matrices& = delete;
  auto operator=(Matrices&&) -> Matrices& = delete;

  void on_update(const basalt::UpdateContext&) override;
  auto name() -> std::string_view override;

private:
  std::shared_ptr<basalt::Scene> mScene = std::make_shared<basalt::Scene>();
  std::shared_ptr<basalt::gfx::SceneView> mSceneView {};
  entt::entity mTriangle {entt::null};
};

} // namespace d3d9
