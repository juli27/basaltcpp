#pragma once

#include <basalt/api/types.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/scene/ecs.h>

#include <basalt/api/shared/types.h>

#include <array>

namespace samples {

class Lighting final : public basalt::View {
public:
  explicit Lighting(basalt::Engine&);

private:
  basalt::gfx::ResourceCachePtr mGfxCache;
  basalt::SecondsF32 mTime {};
  basalt::Entity mCenter {};
  std::array<basalt::Entity, 10> mSpheres {};
  basalt::Entity mGround {};
  basalt::Entity mLight {};

  auto on_update(UpdateContext&) -> void override;
};

} // namespace samples
