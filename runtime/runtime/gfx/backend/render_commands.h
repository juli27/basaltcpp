#pragma once

#include "types.h"

#include <runtime/scene/types.h>
#include <runtime/math/mat4.h>

#include <runtime/shared/asserts.h>
#include <runtime/shared/color.h>
#include <runtime/shared/types.h>

#include <array>

namespace basalt {

struct DirectionalLight;

namespace gfx {

enum class RenderCommandType : u8 {
  Unknown
, RenderCommandLegacy
, SetDirectionalLights
};

struct RenderCommand {
  RenderCommandType type {RenderCommandType::Unknown};

  constexpr RenderCommand() noexcept = default;

  constexpr RenderCommand(const RenderCommand&) noexcept = default;
  constexpr RenderCommand(RenderCommand&&) noexcept = default;

  ~RenderCommand() noexcept = default;

  auto operator=(const RenderCommand&) noexcept -> RenderCommand& = default;
  auto operator=(RenderCommand&&) noexcept -> RenderCommand& = default;

  // TODO: is there a better solution?
  template <typename T>
  auto as() -> T& {
    BASALT_ASSERT_MSG(type == T::TYPE, "invalid input event cast");
    return *static_cast<T*>(this);
  }
};

template <RenderCommandType Type>
struct RenderCommandT : RenderCommand {
  static constexpr RenderCommandType TYPE = Type;

  constexpr RenderCommandT() noexcept
    : RenderCommand {TYPE} {
  }

  constexpr RenderCommandT(const RenderCommandT&) noexcept = default;
  constexpr RenderCommandT(RenderCommandT&&) noexcept = default;

  ~RenderCommandT() noexcept = default;

  auto operator=(const RenderCommandT&) noexcept -> RenderCommandT& = default;
  auto operator=(RenderCommandT&&) noexcept -> RenderCommandT& = default;
};

struct RenderCommandSetDirectionalLights final : RenderCommandT<
    RenderCommandType::SetDirectionalLights> {
  // TODO: tweak maximum
  // can't be a vector. Otherwise it leaks. (no virtual destructor)
  std::array<DirectionalLight, 4> directionalLights {};

  explicit RenderCommandSetDirectionalLights(
    std::array<DirectionalLight, 4> dl) noexcept
    : directionalLights {dl} {
  }

  RenderCommandSetDirectionalLights(const RenderCommandSetDirectionalLights&)
  = default;
  RenderCommandSetDirectionalLights(RenderCommandSetDirectionalLights&&)
  = default;

  ~RenderCommandSetDirectionalLights() noexcept = default;

  auto operator=(
    const RenderCommandSetDirectionalLights&) ->
  RenderCommandSetDirectionalLights& = default;
  auto operator=(
    RenderCommandSetDirectionalLights&&) -> RenderCommandSetDirectionalLights&
  = default;
};

static_assert(sizeof(RenderCommandSetDirectionalLights) == 180);
static_assert(std::is_trivially_destructible_v<RenderCommandSetDirectionalLights>);

enum RenderFlags : u8 {
  RenderFlagNone = 0x0,
  RenderFlagCullNone = 0x1,
  RenderFlagDisableLighting = 0x2
};

enum class TexCoordinateSrc : u8 {
  Vertex, PositionCameraSpace
};


struct RenderCommandLegacy final : RenderCommandT<
    RenderCommandType::RenderCommandLegacy> {
  u8 flags {RenderFlagNone};
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

static_assert(sizeof(RenderCommandLegacy) == 192);
static_assert(std::is_trivially_destructible_v<RenderCommandLegacy>);

} // namespace gfx
} // namespace basalt
