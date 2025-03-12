#pragma once

#include <basalt/api/scene_view.h>

#include "scene_inspector.h"
#include "types.h"

#include <basalt/api/gfx/types.h>

#include <basalt/api/scene/types.h>

#include <vector>

class DebugSceneView final : public basalt::SceneView {
public:
  [[nodiscard]]
  static auto create(basalt::ScenePtr, basalt::gfx::ResourceCachePtr,
                     basalt::EntityId cameraEntity) -> DebugSceneViewPtr;

  DebugSceneView(basalt::ScenePtr, basalt::gfx::ResourceCachePtr,
                 basalt::EntityId cameraEntity);

private:
  auto on_update(UpdateContext&) -> void override;

  bool mInspectorOpen{false};
  SceneInspector mInspector;
};
