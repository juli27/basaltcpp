#pragma once
#ifndef BS_GFX_BACKEND_RENDERCOMMAND_H
#define BS_GFX_BACKEND_RENDERCOMMAND_H

#include <vector>

#include <basalt/common/Color.h>
#include <basalt/math/Mat4.h>

#include "Types.h"

namespace basalt {
namespace gfx {
namespace backend {


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

  inline const std::vector<RenderCommand>& GetCommands() const;

  inline const math::Mat4f32& GetView() const;

  inline const math::Mat4f32& GetProjection() const;

public:
  inline RenderCommandBuffer& operator=(const RenderCommandBuffer&) = delete;

  inline RenderCommandBuffer& operator=(RenderCommandBuffer&&) = default;

private:
  std::vector<RenderCommand> m_commands;
  math::Mat4f32 m_view;
  math::Mat4f32 m_projection;
};


inline RenderCommandBuffer::RenderCommandBuffer()
  : m_view(math::Mat4f32::Identity())
  , m_projection(math::Mat4f32::Identity()) {}


inline RenderCommandBuffer::RenderCommandBuffer(
  const math::Mat4f32& view, const math::Mat4f32& projection
) : m_view(view), m_projection(projection) {}


inline void RenderCommandBuffer::AddCommand(const RenderCommand& command) {
  m_commands.push_back(command);
}


inline void RenderCommandBuffer::Clear() {
  m_commands.clear();
}


inline void RenderCommandBuffer::SetView(const math::Mat4f32& view) {
  m_view = view;
}


inline void RenderCommandBuffer::SetProjection(
  const math::Mat4f32& projection
) {
  m_projection = projection;
}


inline const std::vector<RenderCommand>&
RenderCommandBuffer::GetCommands() const {
  return m_commands;
}


inline const math::Mat4f32& RenderCommandBuffer::GetView() const {
  return m_view;
}


inline const math::Mat4f32& RenderCommandBuffer::GetProjection() const {
  return m_projection;
}

} // namespace backend
} // namespace gfx
} // namespace basalt

#endif // !BS_GFX_BACKEND_RENDERCOMMAND_H
