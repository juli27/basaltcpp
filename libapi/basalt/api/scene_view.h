#pragma once

#include <basalt/api/view.h>

#include <basalt/api/types.h>

#include <basalt/api/gfx/camera.h>

#include <basalt/api/scene/types.h>

namespace basalt {

class SceneView final : public View {
public:
  static auto create(ScenePtr, const gfx::Camera&) -> SceneViewPtr;

  SceneView(ScenePtr, const gfx::Camera&);

  SceneView(const SceneView&) = delete;
  SceneView(SceneView&&) noexcept = default;

  ~SceneView() noexcept override = default;

  auto operator=(const SceneView&) -> SceneView& = delete;
  auto operator=(SceneView&&) noexcept -> SceneView& = default;

  [[nodiscard]] auto camera() const noexcept -> const gfx::Camera&;

private:
  ScenePtr mScene;
  gfx::Camera mCamera;

  auto on_update(UpdateContext&) -> void override;
};

} // namespace basalt
