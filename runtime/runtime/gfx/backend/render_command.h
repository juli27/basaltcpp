#pragma once
#ifndef BASALT_RUNTIME_GFX_BACKEND_RENDER_COMMAND_H
#define BASALT_RUNTIME_GFX_BACKEND_RENDER_COMMAND_H

#include "runtime/gfx/backend/Types.h"
#include "runtime/scene/types.h"
#include "runtime/math/Mat4.h"

#include "runtime/shared/Color.h"
#include "runtime/shared/Types.h"

#include <vector>

namespace basalt::gfx::backend {

enum RenderFlags : u8 {
  RenderFlagNone = 0x0,
  RenderFlagCullNone = 0x1,
  RenderFlagDisableLighting = 0x2
};

enum class TexCoordinateSrc : u8 {
  Vertex, PositionCameraSpace
};


struct RenderCommand final {
  MeshHandle mMesh;
  ModelHandle model;

  // TODO: Material
  Color mDiffuseColor;
  Color mAmbientColor;
  Color mEmissiveColor;
  TextureHandle mTexture;

  math::Mat4f32 mWorld;
  math::Mat4f32 texTransform {math::Mat4f32::identity()};
  TexCoordinateSrc texCoordinateSrc {TexCoordinateSrc::Vertex};
  u8 mFlags = RenderFlagNone;
};


// associates commands with their common transform (camera) and
// defines defaults for render state flags (lighting on/off, ...)
// (TODO: can every state flag be overridden by each command
//        or only some, or none)
struct RenderCommandList final {
  RenderCommandList() = default;
  RenderCommandList(const math::Mat4f32& view, const math::Mat4f32& projection);

  RenderCommandList(const RenderCommandList&) = delete;
  RenderCommandList(RenderCommandList&&) = default;

  ~RenderCommandList() = default;

  auto operator=(const RenderCommandList&) -> RenderCommandList& = delete;
  auto operator=(RenderCommandList&&) -> RenderCommandList& = default;

  [[nodiscard]]
  auto commands() const -> const std::vector<RenderCommand>&;

  [[nodiscard]]
  auto view() const -> const math::Mat4f32&;

  [[nodiscard]]
  auto projection() const -> const math::Mat4f32&;

  [[nodiscard]]
  auto ambient_light() const -> const Color&;
  void set_ambient_light(const Color&);

  [[nodiscard]]
  auto directional_lights() const -> const std::vector<DirectionalLight>&;
  void set_directional_lights(const std::vector<DirectionalLight>&);

  void add(const RenderCommand&);

private:
  std::vector<RenderCommand> mCommands {};
  std::vector<DirectionalLight> mDirectionalLights {};
  math::Mat4f32 mView {math::Mat4f32::identity()};
  math::Mat4f32 mProjection {math::Mat4f32::identity()};
  Color mAmbientLightColor {};
};

} // namespace basalt::gfx::backend

#endif // !BASALT_RUNTIME_GFX_BACKEND_RENDER_COMMAND_H
