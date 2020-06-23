#pragma once

#include "drawable.h"

#include "camera.h"

#include <memory>

namespace basalt {

struct Debug;
struct Scene;

namespace gfx {

// A view into a scene
struct SceneView final : Drawable {
  SceneView(std::shared_ptr<Scene> scene, const Camera& camera);

  SceneView(const SceneView&) = delete;
  SceneView(SceneView&&) = default;

  ~SceneView() override = default;

  auto operator=(const SceneView&) -> SceneView& = delete;
  auto operator=(SceneView&&) -> SceneView& = default;

  auto draw(Size2Du16 viewport) -> CommandList override;

  // TODO: remove
  [[nodiscard]]
  auto clear_color() const -> Color override;

private:
  friend Debug;

  std::shared_ptr<Scene> mScene {};
  Camera mCamera {};
};

} // namespace gfx
} // namespace basalt
