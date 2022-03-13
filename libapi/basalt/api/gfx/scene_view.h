#pragma once

#include <basalt/api/gfx/drawable.h>

#include <basalt/api/gfx/camera.h>

#include <basalt/api/scene/types.h>

#include <memory>

namespace basalt::gfx {

struct SceneView final : Drawable {
  SceneView(ScenePtr scene, const Camera& camera);

  SceneView(const SceneView&) = delete;
  SceneView(SceneView&&) noexcept = default;

  ~SceneView() noexcept override = default;

  auto operator=(const SceneView&) -> SceneView& = delete;
  auto operator=(SceneView&&) noexcept -> SceneView& = default;

  [[nodiscard]] auto camera() const noexcept -> const Camera&;

private:
  ScenePtr mScene;
  Camera mCamera;

  auto on_draw(ResourceCache&, Size2Du16 viewport, const RectangleU16& clip)
    -> std::tuple<CommandList, RectangleU16> override;
};

} // namespace basalt::gfx
