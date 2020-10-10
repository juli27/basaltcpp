#pragma once

#include "api/math/vector3.h"

#include "api/shared/color.h"

namespace basalt {

struct DirectionalLight final {
  Vector3f32 direction {};
  Color diffuseColor {};
  Color ambientColor {};
};

} // namespace basalt
