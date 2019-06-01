#pragma once
#ifndef BS_GFX_LIGHTING_H
#define BS_GFX_LIGHTING_H

#include <vector>

#include <basalt/math/Vec3.h>

namespace basalt {
namespace gfx {
namespace backend {

struct DirectionalLight final {
  math::Vec3f32 direction;
  u32 diffuseColor;
};


class LightSetup final {
public:
  inline LightSetup();

  inline void AddDirectionalLight(math::Vec3f32 direction, u32 diffuseColor);

  inline void SetGlobalAmbientColor(u32 ambientColor);

  inline const std::vector<DirectionalLight>& GetDirectionalLights() const;

  inline u32 GetGlobalAmbientColor() const;

private:
  std::vector<DirectionalLight> m_directionalLights;
  // TODO: color class
  u32 m_ambientColor;
};


inline LightSetup::LightSetup() : m_ambientColor{} {}


inline void LightSetup::AddDirectionalLight(
  math::Vec3f32 direction, u32 diffuseColor
) {
  m_directionalLights.push_back({direction, diffuseColor});
}


inline void LightSetup::SetGlobalAmbientColor(u32 ambientColor) {
  m_ambientColor = ambientColor;
}

inline const std::vector<DirectionalLight>&
LightSetup::GetDirectionalLights() const {
  return m_directionalLights;
}

inline u32 LightSetup::GetGlobalAmbientColor() const {
  return m_ambientColor;
}

} // namespace backend
} // namespace gfx
} // namespace basalt

#endif // !BS_GFX_LIGHTING_H
