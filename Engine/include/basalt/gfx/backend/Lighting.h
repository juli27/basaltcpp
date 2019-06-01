#pragma once
#ifndef BS_GFX_LIGHTING_H
#define BS_GFX_LIGHTING_H

#include <vector>

#include <basalt/math/Vec3.h>

namespace basalt {
namespace gfx {

class DirectionalLight final {
private:
  math::Vec3f32 m_direction;
};


class LightSetup final {
public:
  inline LightSetup();

  inline void SetGlobalAmbientColor(u32 ambientColor);

  inline u32 GetGlobalAmbientColor();

private:
  std::vector<DirectionalLight> m_directionalLights;
  // TODO: color class
  u32 m_ambientColor;
};


inline LightSetup::LightSetup() : m_ambientColor{} {}


inline void LightSetup::SetGlobalAmbientColor(u32 ambientColor) {
  m_ambientColor = ambientColor;
}

inline u32 LightSetup::GetGlobalAmbientColor() {
  return m_ambientColor;
}

} // namespace gfx
} // namespace basalt

#endif // !BS_GFX_LIGHTING_H
