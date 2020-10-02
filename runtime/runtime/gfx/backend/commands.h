#pragma once

#include "types.h"

#include "runtime/scene/types.h"
#include "runtime/math/mat4.h"

#include "runtime/shared/color.h"
#include "runtime/shared/types.h"

#include <array>

namespace basalt::gfx {

struct CommandSetDirectionalLights final
  : CommandT<CommandType::SetDirectionalLights> {
  // TODO: tweak maximum
  // can't be a vector. Otherwise it leaks. (no virtual destructor)
  std::array<DirectionalLight, 4> directionalLights {};

  constexpr explicit CommandSetDirectionalLights(
    std::array<DirectionalLight, 4> dl) noexcept
    : directionalLights {dl} {
  }
};

static_assert(sizeof(CommandSetDirectionalLights) == 180);

struct CommandSetAmbientLight final : CommandT<CommandType::SetAmbientLight> {
  Color ambientColor {};

  constexpr explicit CommandSetAmbientLight(const Color& color) noexcept
    : ambientColor {color} {
  }
};

static_assert(sizeof(CommandSetAmbientLight) == 20);

struct CommandSetTransform final : CommandT<CommandType::SetTransform> {
  TransformType transformType;
  Mat4f32 transform {};

  CommandSetTransform(const TransformType tType, const Mat4f32& t) noexcept
    : transformType {tType}, transform {t} {
  }
};

static_assert(sizeof(CommandSetTransform) == 68);

struct CommandSetRenderState final : CommandT<CommandType::SetRenderState> {
  RenderState renderState;
  u32 value;

  constexpr CommandSetRenderState(const RenderState state, const u32 val) noexcept
    : renderState {state}, value {val} {
  }
};

static_assert(sizeof(CommandSetRenderState) == 8);

struct CommandLegacy final : CommandT<CommandType::Legacy> {
  TexCoordinateSrc texCoordinateSrc {TexCoordinateSrc::Vertex};

  MeshHandle mesh {};
  ModelHandle model {};

  // TODO: Material
  Color diffuseColor {};
  Color ambientColor {};
  Color emissiveColor {};
  TextureHandle texture {};

  Mat4f32 worldTransform {Mat4f32::identity()};
  Mat4f32 texTransform {Mat4f32::identity()};
};

static_assert(sizeof(CommandLegacy) == 192);

} // namespace basalt::gfx
