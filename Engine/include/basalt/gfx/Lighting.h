#pragma once
#ifndef BS_GFX_LIGHTING_H
#define BS_GFX_LIGHTING_H

#include <vector>

#include <basalt/math/Vec3.h>

namespace basalt {
namespace gfx {
namespace backend {

class DirectionalLight final {
private:
  math::Vec3f32 m_direction;
};


class LightSetup final {
private:
  std::vector<DirectionalLight> m_directionalLights;
  // TODO: color class
  u32 m_ambientColor;
};

} // namespace backend
} // namespace gfx
} // namespace basalt

#endif // !BS_GFX_LIGHTING_H
