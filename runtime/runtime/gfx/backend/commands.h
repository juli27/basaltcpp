#pragma once

#include "types.h"

#include <runtime/scene/types.h>
#include <runtime/math/mat4.h>

#include <runtime/shared/color.h>
#include <runtime/shared/types.h>

#include <array>

namespace basalt {

struct DirectionalLight;

namespace gfx {

struct CommandSetDirectionalLights final : CommandT<
    CommandType::SetDirectionalLights> {
  // TODO: tweak maximum
  // can't be a vector. Otherwise it leaks. (no virtual destructor)
  std::array<DirectionalLight, 4> directionalLights {};

  explicit CommandSetDirectionalLights(
    std::array<DirectionalLight, 4> dl) noexcept
    : directionalLights {dl} {
  }

  CommandSetDirectionalLights(const CommandSetDirectionalLights&) = default;
  CommandSetDirectionalLights(CommandSetDirectionalLights&&) = default;

  ~CommandSetDirectionalLights() noexcept = default;

  auto operator=(
    const CommandSetDirectionalLights&) -> CommandSetDirectionalLights&
  = default;
  auto operator=(
    CommandSetDirectionalLights&&) -> CommandSetDirectionalLights& = default;
};

static_assert(sizeof(CommandSetDirectionalLights) == 180);
static_assert(std::is_trivially_destructible_v<CommandSetDirectionalLights>);

struct CommandSetAmbientLight final : CommandT<CommandType::SetAmbientLight> {
  Color ambientColor {};

  explicit CommandSetAmbientLight(const Color& color) noexcept
    : ambientColor {color} {
  }

  CommandSetAmbientLight(const CommandSetAmbientLight&) = default;
  CommandSetAmbientLight(CommandSetAmbientLight&&) = default;

  ~CommandSetAmbientLight() noexcept = default;

  auto operator=(
    const CommandSetAmbientLight&) -> CommandSetAmbientLight& = default;
  auto operator=(CommandSetAmbientLight&&) -> CommandSetAmbientLight& = default;
};

static_assert(sizeof(CommandSetAmbientLight) == 20);
static_assert(std::is_trivially_destructible_v<CommandSetAmbientLight>);

struct CommandSetTransform final : CommandT<CommandType::SetTransform> {
  TransformType transformType;
  Mat4f32 transform {};

  CommandSetTransform(const TransformType tType, const Mat4f32& t) noexcept
    : transformType {tType}, transform {t} {
  }

  CommandSetTransform(const CommandSetTransform&) = default;
  CommandSetTransform(CommandSetTransform&&) = default;

  ~CommandSetTransform() noexcept = default;

  auto operator=(const CommandSetTransform&) -> CommandSetTransform& = default;
  auto operator=(CommandSetTransform&&) -> CommandSetTransform& = default;
};

static_assert(sizeof(CommandSetTransform) == 68);
static_assert(std::is_trivially_destructible_v<CommandSetTransform>);

struct CommandRenderImGui final : CommandT<CommandType::RenderImGui> {
  CommandRenderImGui() noexcept = default;

  CommandRenderImGui(const CommandRenderImGui&) = default;
  CommandRenderImGui(CommandRenderImGui&&) = default;

  ~CommandRenderImGui() noexcept = default;

  auto operator=(const CommandRenderImGui&) -> CommandRenderImGui& = default;
  auto operator=(CommandRenderImGui&&) -> CommandRenderImGui& = default;
};

static_assert(sizeof(CommandRenderImGui) == 1);
static_assert(std::is_trivially_destructible_v<CommandRenderImGui>);

struct CommandLegacy final : CommandT<
    CommandType::Legacy> {
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

static_assert(sizeof(CommandLegacy) == 192);
static_assert(std::is_trivially_destructible_v<CommandLegacy>);

} // namespace gfx
} // namespace basalt
