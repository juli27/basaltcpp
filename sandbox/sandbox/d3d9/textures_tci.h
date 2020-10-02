#pragma once

#include <sandbox/test_case.h>

#include <runtime/gfx/scene_view.h>
#include <runtime/scene/scene.h>

#include <memory>

namespace d3d9 {

struct TexturesTci final : TestCase {
  explicit TexturesTci(basalt::Engine&);

  TexturesTci(const TexturesTci&) = delete;
  TexturesTci(TexturesTci&&) = delete;

  ~TexturesTci() override = default;

  auto operator=(const TexturesTci&) -> TexturesTci& = delete;
  auto operator=(TexturesTci &&) -> TexturesTci& = delete;

  void on_update(const basalt::UpdateContext&) override;
  auto name() -> std::string_view override;

private:
  std::shared_ptr<basalt::Scene> mScene {std::make_shared<basalt::Scene>()};
  std::shared_ptr<basalt::gfx::SceneView> mSceneView {};
  entt::entity mCylinder {entt::null};
};

} // namespace d3d9
