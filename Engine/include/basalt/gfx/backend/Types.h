#pragma once
#ifndef BS_GFX_BACKEND_TYPES_H
#define BS_GFX_BACKEND_TYPES_H

#include <vector>

#include <basalt/common/Color.h>
#include <basalt/common/Handle.h>
#include <basalt/common/Types.h>
#include <basalt/math/Vec3.h>

namespace basalt {
namespace gfx {
namespace backend {


enum class VertexElementUsage : i8 {
  POSITION,
  POSITION_TRANSFORMED,
  NORMAL,
  COLOR_DIFFUSE,
  COLOR_SPECULAR,
  TEXTURE_COORDS
};


enum class VertexElementType : i8 {
  F32_1,
  F32_2,
  F32_3,
  F32_4,
  U32_1
};


struct VertexElement final {
  VertexElementUsage usage;
  VertexElementType type;
};


class VertexLayout final {
public:

  inline VertexLayout(std::initializer_list<VertexElement> elements);

  inline VertexLayout() = default;
  inline VertexLayout(const VertexLayout&) = default;
  inline VertexLayout(VertexLayout&&) = default;
  inline ~VertexLayout() = default;

public:
  inline void AddElement(VertexElementUsage usage, VertexElementType type);


  inline const std::vector<VertexElement>& GetElements() const;

public:
  inline VertexLayout& operator=(const VertexLayout&) = default;
  inline VertexLayout& operator=(VertexLayout&&) = default;

private:
  std::vector<VertexElement> m_elements;
};


inline VertexLayout::VertexLayout(std::initializer_list<VertexElement> elements)
  : m_elements(elements) {}


inline void VertexLayout::AddElement(
  VertexElementUsage usage, VertexElementType type
) {
  m_elements.push_back({usage, type});
}


inline const std::vector<VertexElement>& VertexLayout::GetElements() const {
  return m_elements;
}


enum class PrimitiveType : i8 {
  POINT_LIST,
  LINE_LIST,
  LINE_STRIP,
  TRIANGLE_LIST,
  TRIANGLE_STRIP,
  TRIANGLE_FAN
};


using MeshHandle = Handle<HandleTarget::GFX_MESH>;


using TextureHandle = Handle<HandleTarget::GFX_TEXTURE>;


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

#endif // !BS_GFX_BACKEND_TYPES_H
