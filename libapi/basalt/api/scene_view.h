#pragma once

#include <basalt/api/gfx/drawable.h>

#include <basalt/api/gfx/camera.h>

#include <basalt/api/scene/types.h>

#include <memory>

namespace basalt {

struct SceneView final : gfx::Drawable {
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

  auto on_draw(gfx::ResourceCache&, Size2Du16 viewport,
               const RectangleU16& clip)
    -> std::tuple<gfx::CommandList, RectangleU16> override;
};

} // namespace basalt
