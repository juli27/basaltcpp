#pragma once
#ifndef BASALT_GFX_BACKEND_TYPES_H
#define BASALT_GFX_BACKEND_TYPES_H

#include <basalt/math/Vec3.h>

#include <basalt/shared/Color.h>
#include <basalt/shared/Handle.h>
#include <basalt/shared/Types.h>

#include <vector>

namespace basalt::gfx::backend {

enum class VertexElementUsage : i8 {
  Position,
  PositionTransformed,
  Normal,
  ColorDiffuse,
  ColorSpecular,
  TextureCoords
};


enum class VertexElementType : i8 {
  F32_1,
  F32_2,
  F32_3,
  F32_4,
  U32_1
};


struct VertexElement final {
  VertexElementUsage mUsage;
  VertexElementType mType;
};


struct VertexLayout final {
  VertexLayout() = default;
  inline VertexLayout(std::initializer_list<VertexElement> elements);
  VertexLayout(const VertexLayout&) = default;
  VertexLayout(VertexLayout&&) = default;
  ~VertexLayout() = default;

  auto operator=(const VertexLayout&) -> VertexLayout& = default;
  auto operator=(VertexLayout&&) -> VertexLayout& = default;

  inline void add_element(VertexElementUsage usage, VertexElementType type);

  [[nodiscard]]
  inline auto get_elements() const -> const std::vector<VertexElement>&;

private:
  std::vector<VertexElement> mElements;
};

inline VertexLayout::VertexLayout(
  const std::initializer_list<VertexElement> elements
)
: mElements(elements) {}

inline void VertexLayout::add_element(
  const VertexElementUsage usage, const VertexElementType type
) {
  mElements.push_back({usage, type});
}

inline auto VertexLayout::get_elements() const
-> const std::vector<VertexElement>& {
  return mElements;
}


enum class PrimitiveType : i8 {
  PointList,
  LineList,
  LineStrip,
  TriangleList,
  TriangleStrip,
  TriangleFan
};


namespace _internal {

struct MeshTypeTag {};
struct TextureTypeTag {};

}


using MeshHandle = Handle<_internal::MeshTypeTag>;
using TextureHandle = Handle<_internal::TextureTypeTag>;


struct DirectionalLight final {
  math::Vec3f32 mDirection;
  Color mDiffuseColor;
  Color mAmbientColor;
};


struct LightSetup final {
  LightSetup() = default;
  LightSetup(const LightSetup&) = default;
  LightSetup(LightSetup&&) = default;
  ~LightSetup() = default;

  auto operator=(const LightSetup&) -> LightSetup& = default;
  auto operator=(LightSetup&&) -> LightSetup& = default;

  inline void add_directional_light(math::Vec3f32 direction, Color diffuseColor);

  inline void set_global_ambient_color(Color ambientColor);

  [[nodiscard]]
  inline auto get_directional_lights() const
    -> const std::vector<DirectionalLight>&;
  [[nodiscard]] inline auto get_global_ambient_color() const -> Color;

private:
  std::vector<DirectionalLight> mDirectionalLights;
  Color mAmbientColor;
};

inline void LightSetup::add_directional_light(
  const math::Vec3f32 direction, const Color diffuseColor
) {
  mDirectionalLights.push_back({direction, diffuseColor});
}

inline void LightSetup::set_global_ambient_color(const Color ambientColor) {
  mAmbientColor = ambientColor;
}

inline auto LightSetup::get_directional_lights() const
-> const std::vector<DirectionalLight>& {
  return mDirectionalLights;
}

inline auto LightSetup::get_global_ambient_color() const -> Color {
  return mAmbientColor;
}

} // namespace basalt::gfx::backend

#endif // !BASALT_GFX_BACKEND_TYPES_H
