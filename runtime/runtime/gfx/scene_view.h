#pragma once

#include "Camera.h"
#include "drawable.h"
#include "backend/render_command.h"

#include <runtime/scene/scene.h>

#include <memory>

namespace basalt::gfx {

struct SceneView final : Drawable {
  SceneView(std::shared_ptr<Scene> scene, const Camera& camera);

  SceneView(const SceneView&) = delete;
  SceneView(SceneView&&) = default;

  ~SceneView() override = default;

  auto operator=(const SceneView&) -> SceneView& = delete;
  auto operator=(SceneView&&) -> SceneView& = default;

  auto draw(Size2Du16 viewport) -> backend::RenderCommandList override;

  // TODO: remove
  [[nodiscard]]
  auto clear_color() const -> Color override;

private:
  friend struct ::basalt::Debug;

  std::shared_ptr<Scene> mScene {};
  Camera mCamera {};
};

} // namespace basalt::gfx
