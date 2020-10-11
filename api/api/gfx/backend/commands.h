#pragma once

#include "types.h"

#include "api/scene/types.h"

#include "api/math/mat4.h"

#include "api/shared/color.h"

#include "api/base/types.h"

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

struct CommandSetTransform final : CommandT<CommandType::SetTransform> {
  TransformState state;
  Mat4f32 transform;

  CommandSetTransform(const TransformState s, const Mat4f32& t) noexcept
    : state {s}, transform {t} {
  }
};

static_assert(sizeof(CommandSetTransform) == 68);

struct CommandSetRenderState final : CommandT<CommandType::SetRenderState> {
  RenderState renderState;
  u32 value;

  constexpr CommandSetRenderState(const RenderState state,
                                  const u32 val) noexcept
    : renderState {state}, value {val} {
  }
};

static_assert(sizeof(CommandSetRenderState) == 8);

struct CommandSetTextureStageState final
  : CommandT<CommandType::SetTextureStageState> {
  u8 stage;
  TextureStageState state;
  u32 value;

  constexpr CommandSetTextureStageState(
    const u8 textureStage, const TextureStageState textureStageState,
    const u32 stateValue) noexcept
    : stage {textureStage}, state {textureStageState}, value {stateValue} {
  }
};

static_assert(sizeof(CommandSetTextureStageState) == 8);

struct CommandLegacy final : CommandT<CommandType::Legacy> {
  TexCoordinateSrc texCoordinateSrc {TcsVertex};

  MeshHandle mesh {MeshHandle::null()};

  // TODO: Material
  Color diffuseColor;
  Color ambientColor;
  Color emissiveColor;
  TextureHandle texture;
};

static_assert(sizeof(CommandLegacy) == 60);

} // namespace basalt::gfx
