#pragma once

#include <basalt/api/scene/system.h>
#include <basalt/api/scene/types.h>

#include <basalt/api/base/types.h>

struct RotationSpeed {
  basalt::f32 xRadPerSecond;
  basalt::f32 yRadPerSecond;
  basalt::f32 zRadPerSecond;
};

class RotationSystem final : public basalt::System {
public:
  using UpdateBefore = basalt::TransformSystem;

  auto on_update(UpdateContext const& ctx) -> void override;
};
