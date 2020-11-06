#pragma once

#include "drawable.h"

#include "camera.h"

#include "api/scene/types.h"

#include <memory>

namespace basalt::gfx {

struct SceneView final : Drawable {
  SceneView(ScenePtr scene, const Camera& camera);

  SceneView(const SceneView&) = delete;
  SceneView(SceneView&&) = default;

  ~SceneView() override = default;

  auto operator=(const SceneView&) -> SceneView& = delete;
  auto operator=(SceneView &&) -> SceneView& = default;

  [[nodiscard]] auto camera() const noexcept -> const Camera&;

  auto draw(ResourceCache&, Size2Du16 viewport, const RectangleU16& clip)
    -> std::tuple<CommandList, RectangleU16> override;

private:
  ScenePtr mScene;
  Camera mCamera;
};

} // namespace basalt::gfx
