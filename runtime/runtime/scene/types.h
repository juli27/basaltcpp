#pragma once
#ifndef BASALT_RUNTIME_SCENE_TYPES_H
#define BASALT_RUNTIME_SCENE_TYPES_H

#include "runtime/math/Vec3.h"
#include "runtime/shared/Color.h"

namespace basalt {

struct DirectionalLight final {
  math::Vec3f32 direction {};
  Color diffuseColor {};
  Color ambientColor {};
};

} // namespace basalt

#endif // BASALT_RUNTIME_SCENE_TYPES_H
