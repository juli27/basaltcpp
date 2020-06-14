#pragma once

#include <sandbox/test_case.h>

#include <runtime/gfx/backend/device.h>
#include <runtime/gfx/scene_view.h>
#include <runtime/scene/scene.h>

#include <memory>

namespace d3d9 {

struct Vertices final : TestCase {
  Vertices() = delete;
  explicit Vertices(basalt::gfx::Device&);

  Vertices(const Vertices&) = delete;
  Vertices(Vertices&&) = delete;

  ~Vertices() override = default;

  auto operator=(const Vertices&) -> Vertices& = delete;
  auto operator=(Vertices&&) -> Vertices& = delete;

  void on_update(const basalt::UpdateContext&) override;
  auto name() -> std::string_view override;

private:
  std::shared_ptr<basalt::Scene> mScene {std::make_shared<basalt::Scene>()};
  std::shared_ptr<basalt::gfx::SceneView> mSceneView {};
};

} // namespace d3d9
