#pragma once

#include "runtime/math/vec3.h"
#include "runtime/shared/color.h"

namespace basalt {

struct DirectionalLight final {
  Vec3f32 direction {};
  Color diffuseColor {};
  Color ambientColor {};
};

} // namespace basalt
