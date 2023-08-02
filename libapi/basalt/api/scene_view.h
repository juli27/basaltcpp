#pragma once

#include <basalt/api/debug_ui.h>
#include <basalt/api/types.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/scene/types.h>

#include <basalt/api/shared/types.h>

#include <vector>

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

  EntityId mSelectedEntity;
  std::vector<DebugUi::ComponentUi> mComponentUis;

  auto update_debug_ui(Config& config) -> void;

  auto on_input(const InputEvent&) -> InputEventHandled override;
  auto on_update(UpdateContext&) -> void override;
};

} // namespace basalt
