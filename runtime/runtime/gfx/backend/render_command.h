#pragma once

#include "types.h"

#include <runtime/scene/types.h>
#include <runtime/math/mat4.h>

#include <runtime/shared/asserts.h>
#include <runtime/shared/color.h>
#include <runtime/shared/types.h>

#include <memory>
#include <vector>

namespace basalt {

struct DirectionalLight;

namespace gfx {

enum class RenderCommandType : u8 {
  Unknown
, RenderCommandLegacy
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

enum RenderFlags : u8 {
  RenderFlagNone = 0x0,
  RenderFlagCullNone = 0x1,
  RenderFlagDisableLighting = 0x2
};

enum class TexCoordinateSrc : u8 {
  Vertex, PositionCameraSpace
};


struct RenderCommandLegacy final : RenderCommandT<RenderCommandType::RenderCommandLegacy> {
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

using RenderCommandPtr = std::unique_ptr<RenderCommand>;

// associates commands with their common transform (camera) and
// defines defaults for render state flags (lighting on/off, ...)
// (TODO: can every state flag be overridden by each command
//        or only some, or none)
struct CommandList final {
  CommandList() = default;
  CommandList(
    const Mat4f32& view, const Mat4f32& projection
  , const Color& clearColor);

  CommandList(const CommandList&) = delete;
  CommandList(CommandList&&) = default;

  ~CommandList() = default;

  auto operator=(const CommandList&) -> CommandList& = delete;
  auto operator=(CommandList&&) -> CommandList& = default;

  [[nodiscard]]
  auto commands() const -> const std::vector<RenderCommandPtr>&;

  [[nodiscard]]
  auto view() const -> const Mat4f32&;

  [[nodiscard]]
  auto projection() const -> const Mat4f32&;

  [[nodiscard]]
  auto ambient_light() const -> const Color&;
  void set_ambient_light(const Color&);

  [[nodiscard]]
  auto directional_lights() const -> const std::vector<DirectionalLight>&;
  void set_directional_lights(const std::vector<DirectionalLight>&);

  [[nodiscard]]
  auto clear_color() const -> const Color&;

  void add(const RenderCommandLegacy&);

private:
  std::vector<RenderCommandPtr> mCommands {};
  std::vector<DirectionalLight> mDirectionalLights {};
  Mat4f32 mView {Mat4f32::identity()};
  Mat4f32 mProjection {Mat4f32::identity()};
  // TODO: drawable need to be able to clear their area of the draw target
  Color mClearColor {};
  Color mAmbientLightColor {};
};

} // namespace gfx
} // namespace basalt
