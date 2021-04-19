#pragma once

#include <basalt/api/math/vector3.h>

#include <basalt/api/shared/color.h>

#include <memory>

namespace basalt {

struct Scene;
using ScenePtr = std::shared_ptr<Scene>;

struct DirectionalLight final {
  Vector3f32 direction;
  Color diffuseColor;
  Color ambientColor;
};

} // namespace basalt
