#pragma once

#include <basalt/api/view.h>

#include <basalt/api/gfx/camera.h>

#include <basalt/api/scene/types.h>

namespace basalt {

struct SceneView final : View {
  SceneView(ScenePtr scene, const gfx::Camera& camera);

  SceneView(const SceneView&) = delete;
  SceneView(SceneView&&) noexcept = default;

  ~SceneView() noexcept override = default;

  auto operator=(const SceneView&) -> SceneView& = delete;
  auto operator=(SceneView&&) noexcept -> SceneView& = default;

  [[nodiscard]] auto camera() const noexcept -> const gfx::Camera&;

private:
  ScenePtr mScene;
  gfx::Camera mCamera;

  auto on_draw(const DrawContext&) -> void override;
  auto on_tick(Engine&) -> void override;
};

} // namespace basalt
