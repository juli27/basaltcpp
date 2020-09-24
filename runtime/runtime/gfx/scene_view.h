#pragma once

#include "drawable.h"

#include "camera.h"
#include "backend/types.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace basalt {

struct Debug;
struct Scene;

namespace gfx {

struct SceneView final : Drawable {
  SceneView(std::shared_ptr<Scene> scene, const Camera& camera);

  SceneView(const SceneView&) = delete;
  SceneView(SceneView&&) = default;

  ~SceneView() override = default;

  auto operator=(const SceneView&) -> SceneView& = delete;
  auto operator=(SceneView&&) -> SceneView& = default;

  auto draw(Device&, Size2Du16 viewport) -> CommandList override;

  // TODO: remove
  [[nodiscard]]
  auto clear_color() const -> Color override;

private:
  friend Debug;

  std::unordered_map<std::string, ModelHandle> mModelCache {};
  std::shared_ptr<Scene> mScene {};
  Camera mCamera {};
};

} // namespace gfx
} // namespace basalt
