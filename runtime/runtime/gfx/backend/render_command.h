#pragma once

#include "types.h"

#include <runtime/scene/types.h>
#include <runtime/math/mat4.h>

#include <runtime/shared/color.h>
#include <runtime/shared/types.h>

#include <vector>

namespace basalt {

struct DirectionalLight;

namespace gfx {

enum RenderFlags : u8 {
  RenderFlagNone = 0x0,
  RenderFlagCullNone = 0x1,
  RenderFlagDisableLighting = 0x2
};

enum class TexCoordinateSrc : u8 {
  Vertex, PositionCameraSpace
};


struct RenderCommand final {
  MeshHandle mesh {};
  ModelHandle model {};

  // TODO: Material
  Color diffuseColor {};
  Color ambientColor {};
  Color emissiveColor {};
  TextureHandle texture {};

  Mat4f32 worldTransform {Mat4f32::identity()};
  Mat4f32 texTransform {Mat4f32::identity()};
  TexCoordinateSrc texCoordinateSrc {TexCoordinateSrc::Vertex};
  u8 flags {RenderFlagNone};
};

static_assert(sizeof(RenderCommand) == 192);


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
  auto commands() const -> const std::vector<RenderCommand>&;

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

  void add(const RenderCommand&);

private:
  std::vector<RenderCommand> mCommands {};
  std::vector<DirectionalLight> mDirectionalLights {};
  Mat4f32 mView {Mat4f32::identity()};
  Mat4f32 mProjection {Mat4f32::identity()};
  // TODO: drawable need to be able to clear their area of the draw target
  Color mClearColor {};
  Color mAmbientLightColor {};
};

} // namespace gfx
} // namespace basalt
