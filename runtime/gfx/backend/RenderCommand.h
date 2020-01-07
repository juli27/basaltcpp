#pragma once
#ifndef BASALT_GFX_BACKEND_RENDERCOMMAND_H
#define BASALT_GFX_BACKEND_RENDERCOMMAND_H

#include "runtime/math/Mat4.h"
#include "runtime/shared/Color.h"

#include "Types.h"

#include <vector>

namespace basalt::gfx::backend {

enum RenderFlags : u8 {
  RenderFlagNone = 0,
  RenderFlagCullNone = 1,
  RenderFlagDisableLighting = 1 << 1
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
struct RenderCommandBuffer final {
  inline RenderCommandBuffer();
  inline RenderCommandBuffer(
    const math::Mat4f32& view, const math::Mat4f32& projection
  );
  RenderCommandBuffer(const RenderCommandBuffer&) = delete;
  RenderCommandBuffer(RenderCommandBuffer&&) = default;
  ~RenderCommandBuffer() = default;

  auto operator=(const RenderCommandBuffer&) -> RenderCommandBuffer& = delete;
  auto operator=(RenderCommandBuffer&&) -> RenderCommandBuffer& = default;

  inline void add_command(const RenderCommand& command);
  inline void clear();
  inline void set_view(const math::Mat4f32& view);
  inline void set_projection(const math::Mat4f32& projection);

  [[nodiscard]]
  inline auto get_commands() const -> const std::vector<RenderCommand>&;
  [[nodiscard]] inline auto view() const -> const math::Mat4f32&;
  [[nodiscard]] inline auto projection() const -> const math::Mat4f32&;

private:
  std::vector<RenderCommand> mCommands;
  math::Mat4f32 mView;
  math::Mat4f32 mProjection;
};

inline RenderCommandBuffer::RenderCommandBuffer()
  : mView(math::Mat4f32::identity())
  , mProjection(math::Mat4f32::identity()) {}

inline RenderCommandBuffer::RenderCommandBuffer(
  const math::Mat4f32& view, const math::Mat4f32& projection
) : mView(view), mProjection(projection) {}

inline void RenderCommandBuffer::add_command(const RenderCommand& command) {
  mCommands.push_back(command);
}

inline void RenderCommandBuffer::clear() {
  mCommands.clear();
}

inline void RenderCommandBuffer::set_view(const math::Mat4f32& view) {
  mView = view;
}

inline void RenderCommandBuffer::set_projection(
  const math::Mat4f32& projection
) {
  mProjection = projection;
}

inline auto RenderCommandBuffer::get_commands() const
-> const std::vector<RenderCommand>& {
  return mCommands;
}

inline const math::Mat4f32& RenderCommandBuffer::view() const {
  return mView;
}

inline const math::Mat4f32& RenderCommandBuffer::projection() const {
  return mProjection;
}

} // namespace basalt::gfx::backend

#endif // !BASALT_GFX_BACKEND_RENDERCOMMAND_H
