#pragma once

#include <basalt/api/types.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/scene/types.h>

namespace basalt {

class SceneView final : public View {
public:
  static auto create(ScenePtr, gfx::ResourceCachePtr, EntityId cameraEntity)
    -> SceneViewPtr;

  SceneView(const SceneView&) = delete;
  SceneView(SceneView&&) noexcept = default;

  ~SceneView() noexcept override = default;

  auto operator=(const SceneView&) -> SceneView& = delete;
  auto operator=(SceneView&&) noexcept -> SceneView& = default;

  explicit SceneView(ScenePtr, gfx::ResourceCachePtr);

private:
  ScenePtr mScene;
  gfx::ResourceCachePtr mGfxCache;

  auto on_input(const InputEvent&) -> InputEventHandled override;
  auto on_update(UpdateContext&) -> void override;
};

} // namespace basalt
