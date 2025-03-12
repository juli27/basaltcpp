#pragma once

#include "view.h"

#include "types.h"
#include "gfx/types.h"

#include "scene/types.h"

namespace basalt {

class SceneView : public View {
public:
  static auto create(ScenePtr, gfx::ResourceCachePtr, EntityId cameraEntity)
    -> SceneViewPtr;

  SceneView(SceneView const&) = delete;
  SceneView(SceneView&&) noexcept = default;

  ~SceneView() noexcept override = default;

  auto operator=(SceneView const&) -> SceneView& = delete;
  auto operator=(SceneView&&) noexcept -> SceneView& = default;

  SceneView(ScenePtr, gfx::ResourceCachePtr, EntityId cameraEntity);

  [[nodiscard]]
  auto scene() const -> ScenePtr const&;

protected:
  auto on_input(InputEvent const&) -> InputEventHandled override;
  auto on_update(UpdateContext&) -> void override;

private:
  ScenePtr mScene;
  gfx::ResourceCachePtr mGfxCache;
};

} // namespace basalt
