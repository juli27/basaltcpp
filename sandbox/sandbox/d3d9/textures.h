#pragma once

#include "sandbox/test_case.h"

#include <runtime/gfx/backend/IRenderer.h>
#include <runtime/gfx/scene_view.h>
#include <runtime/scene/scene.h>

#include <memory>

namespace d3d9 {

struct Textures final : TestCase {
  Textures() = delete;
  explicit Textures(basalt::gfx::backend::IRenderer&);

  Textures(const Textures&) = delete;
  Textures(Textures&&) = delete;

  ~Textures() override = default;

  auto operator=(const Textures&) -> Textures& = delete;
  auto operator=(Textures&&) -> Textures& = delete;

  void on_update(const basalt::UpdateContext&) override;
  auto name() -> std::string_view override;

private:
  std::shared_ptr<basalt::Scene> mScene {std::make_shared<basalt::Scene>()};
  std::shared_ptr<basalt::gfx::SceneView> mSceneView {};
  entt::entity mCylinder {entt::null};
};

} // namespace d3d9
