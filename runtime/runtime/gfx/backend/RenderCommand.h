#pragma once
#ifndef BASALT_GFX_BACKEND_RENDERCOMMAND_H
#define BASALT_GFX_BACKEND_RENDERCOMMAND_H

#include "runtime/gfx/backend/Types.h"
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


struct RenderCommand final {
  MeshHandle mMesh;

  // TODO: Material
  Color mDiffuseColor;
  Color mAmbientColor;
  Color mEmissiveColor;
  TextureHandle mTexture;

  math::Mat4f32 mWorld;
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

  inline void set_view(const math::Mat4f32& view);

  [[nodiscard]]
  auto view() const -> const math::Mat4f32&;

  inline void set_projection(const math::Mat4f32& projection);

  [[nodiscard]]
  auto projection() const -> const math::Mat4f32&;

  [[nodiscard]]
  auto ambient_light() const -> const Color&;
  void set_ambient_light(const Color&);

  void add(const RenderCommand&);
  void clear();

private:
  std::vector<RenderCommand> mCommands {};
  math::Mat4f32 mView {math::Mat4f32::identity()};
  math::Mat4f32 mProjection {math::Mat4f32::identity()};
  Color mAmbientLightColor {};
};

inline void RenderCommandList::set_view(const math::Mat4f32& view) {
  mView = view;
}

inline void
RenderCommandList::set_projection(const math::Mat4f32& projection) {
  mProjection = projection;
}

} // namespace basalt::gfx::backend

#endif // !BASALT_GFX_BACKEND_RENDERCOMMAND_H
