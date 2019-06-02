#pragma once
#ifndef BS_GFX_LIGHTING_H
#define BS_GFX_LIGHTING_H

#include <vector>

#include <basalt/common/Color.h>
#include <basalt/math/Vec3.h>

namespace basalt {
namespace gfx {
namespace backend {

struct DirectionalLight final {
  math::Vec3f32 direction;
  Color diffuseColor;
  Color ambientColor;
};


class LightSetup final {
public:
  inline LightSetup();

  inline void AddDirectionalLight(math::Vec3f32 direction, Color diffuseColor);

  inline void SetGlobalAmbientColor(Color ambientColor);

  inline const std::vector<DirectionalLight>& GetDirectionalLights() const;

  inline Color GetGlobalAmbientColor() const;

private:
  std::vector<DirectionalLight> m_directionalLights;
  // TODO: color class
  Color m_ambientColor;
};


inline LightSetup::LightSetup() : m_ambientColor{} {}


inline void LightSetup::AddDirectionalLight(
  math::Vec3f32 direction, Color diffuseColor
) {
  m_directionalLights.push_back({direction, diffuseColor});
}


inline void LightSetup::SetGlobalAmbientColor(Color ambientColor) {
  m_ambientColor = ambientColor;
}

inline const std::vector<DirectionalLight>&
LightSetup::GetDirectionalLights() const {
  return m_directionalLights;
}

inline Color LightSetup::GetGlobalAmbientColor() const {
  return m_ambientColor;
}

} // namespace backend
} // namespace gfx
} // namespace basalt

#endif // !BS_GFX_LIGHTING_H
