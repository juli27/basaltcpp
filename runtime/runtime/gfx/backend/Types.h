#pragma once
#ifndef BASALT_GFX_BACKEND_TYPES_H
#define BASALT_GFX_BACKEND_TYPES_H

#include "runtime/math/Vec3.h"

#include "runtime/shared/Color.h"
#include "runtime/shared/Handle.h"
#include "runtime/shared/Types.h"

#include <vector>

namespace basalt::gfx::backend {

enum class VertexElement : u8 {
  Position3F32,
  PositionTransformed4F32,
  Normal3F32,
  ColorDiffuse1U32,
  ColorSpecular1U32,
  TextureCoords2F32
};


using VertexLayout = std::vector<VertexElement>;


enum class PrimitiveType : u8 {
  PointList,
  LineList,
  LineStrip,
  TriangleList,
  TriangleStrip,
  TriangleFan
};


namespace _internal {

struct MeshTypeTag {
};

struct TextureTypeTag {
};

} // namespace _internal


using MeshHandle = Handle<_internal::MeshTypeTag>;
using TextureHandle = Handle<_internal::TextureTypeTag>;


struct DirectionalLight final {
  math::Vec3f32 direction;
  Color diffuseColor;
  Color ambientColor;
};


struct LightSetup final {
  LightSetup() = default;
  LightSetup(const LightSetup&) = default;
  LightSetup(LightSetup&&) = default;
  ~LightSetup() = default;

  auto operator=(const LightSetup&) -> LightSetup& = default;
  auto operator=(LightSetup&&) -> LightSetup& = default;

  void add_directional_light(
    const math::Vec3f32& direction, const Color& diffuseColor
  );

  void set_global_ambient_color(const Color& ambientColor);

  [[nodiscard]]
  auto directional_lights() const -> const std::vector<DirectionalLight>&;

  [[nodiscard]]
  auto global_ambient_color() const -> Color;

private:
  std::vector<DirectionalLight> mDirectionalLights;
  Color mAmbientColor;
};

} // namespace basalt::gfx::backend

#endif // !BASALT_GFX_BACKEND_TYPES_H
