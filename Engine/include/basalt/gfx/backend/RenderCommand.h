#pragma once
#ifndef BS_GFX_BACKEND_RENDERCOMMAND_H
#define BS_GFX_BACKEND_RENDERCOMMAND_H

#include <vector>

#include <basalt/common/Color.h>
#include <basalt/math/Mat4.h>

#include "Types.h"

namespace basalt::gfx::backend {


enum RenderFlags : i8 {
  RF_NONE = 0x00,
  RF_CULL_NONE = 0x01,
  RF_DISABLE_LIGHTING = 0x02
};


struct RenderCommand final {
  MeshHandle mesh;

  // TODO: Material
  Color diffuseColor;
  Color ambientColor;
  Color emissiveColor;
  TextureHandle texture;

  math::Mat4f32 world;
  i8 flags;
};

// associates commands with their common transform (camera) and
// defines defaults for render state flags (lighting on/off, ...)
// (TODO: can every state flag be overridden by each command
//        or only some, or none)
class RenderCommandBuffer final {
public:
  inline RenderCommandBuffer();

  inline RenderCommandBuffer(
    const math::Mat4f32& view, const math::Mat4f32& projection
  );

  inline RenderCommandBuffer(const RenderCommandBuffer&) = delete;

  inline RenderCommandBuffer(RenderCommandBuffer&&) = default;

  inline ~RenderCommandBuffer() = default;

public:
  inline void AddCommand(const RenderCommand& command);

  inline void Clear();

  inline void SetView(const math::Mat4f32& view);

  inline void SetProjection(const math::Mat4f32& projection);

  inline auto GetCommands() const -> const std::vector<RenderCommand>&;

  inline auto GetView() const -> const math::Mat4f32&;

  inline auto GetProjection() const -> const math::Mat4f32&;

public:
  inline auto operator=(const RenderCommandBuffer&) -> RenderCommandBuffer& = delete;

  inline auto operator=(RenderCommandBuffer&&) -> RenderCommandBuffer& = default;

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


inline void RenderCommandBuffer::AddCommand(const RenderCommand& command) {
  mCommands.push_back(command);
}


inline void RenderCommandBuffer::Clear() {
  mCommands.clear();
}


inline void RenderCommandBuffer::SetView(const math::Mat4f32& view) {
  mView = view;
}


inline void RenderCommandBuffer::SetProjection(
  const math::Mat4f32& projection
) {
  mProjection = projection;
}


inline auto RenderCommandBuffer::GetCommands() const
-> const std::vector<RenderCommand>& {
  return mCommands;
}


inline const math::Mat4f32& RenderCommandBuffer::GetView() const {
  return mView;
}


inline const math::Mat4f32& RenderCommandBuffer::GetProjection() const {
  return mProjection;
}

} // namespace basalt::gfx::backend

#endif // !BS_GFX_BACKEND_RENDERCOMMAND_H
