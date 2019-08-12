#pragma once
#ifndef BS_GFX_BACKEND_TYPES_H
#define BS_GFX_BACKEND_TYPES_H

#include <vector>

#include <basalt/common/Color.h>
#include <basalt/common/Handle.h>
#include <basalt/common/Types.h>
#include <basalt/math/Vec3.h>

namespace basalt::gfx::backend {


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


  inline auto GetElements() const -> const std::vector<VertexElement>&;

public:
  inline auto operator=(const VertexLayout&) -> VertexLayout& = default;
  inline auto operator=(VertexLayout&&) -> VertexLayout& = default;

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


inline auto VertexLayout::GetElements() const
-> const std::vector<VertexElement>& {
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


namespace _internal {

struct MeshTypeTag {};
struct TextureTypeTag {};

}


using MeshHandle = Handle<_internal::MeshTypeTag>;


using TextureHandle = Handle<_internal::TextureTypeTag>;


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

  inline auto GetDirectionalLights() const -> const std::vector<DirectionalLight>&;

  inline auto GetGlobalAmbientColor() const -> Color;

private:
  std::vector<DirectionalLight> mDirectionalLights;
  Color mAmbientColor;
};


inline LightSetup::LightSetup() : mAmbientColor{} {}


inline void LightSetup::AddDirectionalLight(
  math::Vec3f32 direction, Color diffuseColor
) {
  mDirectionalLights.push_back({direction, diffuseColor});
}


inline void LightSetup::SetGlobalAmbientColor(Color ambientColor) {
  mAmbientColor = ambientColor;
}


inline auto LightSetup::GetDirectionalLights() const
-> const std::vector<DirectionalLight>& {
  return mDirectionalLights;
}


inline auto LightSetup::GetGlobalAmbientColor() const -> Color {
  return mAmbientColor;
}

} // namespace basalt::gfx::backend

#endif // !BS_GFX_BACKEND_TYPES_H
