#pragma once

#include "api/math/vec3.h"
#include "api/shared/color.h"

namespace basalt {

struct DirectionalLight final {
  Vec3f32 direction {};
  Color diffuseColor {};
  Color ambientColor {};
};

} // namespace basalt
