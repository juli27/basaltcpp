#include "rotation_system.h"

#include <basalt/api/scene/scene.h>
#include <basalt/api/scene/transform.h>

#include <basalt/api/math/angle.h>

using namespace basalt;

auto RotationSystem::on_update(UpdateContext const& ctx) -> void {
  auto const dt = ctx.deltaTime.count();

  ctx.scene.entity_registry().view<Transform, RotationSpeed const>().each(
    [&](Transform& t, RotationSpeed const& rotationSpeed) {
      t.rotate(Angle::radians(rotationSpeed.xRadPerSecond * dt),
               Angle::radians(rotationSpeed.yRadPerSecond * dt),
               Angle::radians(rotationSpeed.zRadPerSecond * dt));
    });
}
