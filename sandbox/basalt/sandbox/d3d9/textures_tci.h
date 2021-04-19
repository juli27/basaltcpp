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

  TexturesTci(const TexturesTci&) = delete;
  TexturesTci(TexturesTci&&) = delete;

  ~TexturesTci() override = default;

  auto operator=(const TexturesTci&) -> TexturesTci& = delete;
  auto operator=(TexturesTci &&) -> TexturesTci& = delete;

  void on_update(const basalt::UpdateContext&) override;
  auto name() -> std::string_view override;

private:
  std::shared_ptr<basalt::Scene> mScene = std::make_shared<basalt::Scene>();
  std::shared_ptr<basalt::gfx::SceneView> mSceneView =
    std::make_shared<basalt::gfx::SceneView>(mScene, create_default_camera());
  entt::handle mCylinder = mScene->create_entity();
};

} // namespace d3d9
