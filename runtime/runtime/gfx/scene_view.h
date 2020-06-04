#pragma once

#include "Camera.h"
#include "backend/render_command.h"

#include <runtime/scene/scene.h>

#include <memory>

namespace basalt::gfx {

struct SceneView final {
  SceneView(std::shared_ptr<Scene> scene, const Camera& camera);

  SceneView(const SceneView&) = delete;
  SceneView(SceneView&&) = default;

  ~SceneView() = default;

  auto operator=(const SceneView&) -> SceneView& = delete;
  auto operator=(SceneView&&) -> SceneView& = default;

  auto draw() const -> backend::RenderCommandList;

  // TODO: remove
  auto clear_color() const -> const Color&;

private:
  friend struct Debug;

  std::shared_ptr<Scene> mScene {};
  Camera mCamera {};
};

} // namespace basalt::gfx
