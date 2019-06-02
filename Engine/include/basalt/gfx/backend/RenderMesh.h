#pragma once
#ifndef BS_GFX_BACKEND_VERTEX_BUFFER_H
#define BS_GFX_BACKEND_VERTEX_BUFFER_H

#include <optional>
#include <vector>

#include <basalt/common/Handle.h>
#include <basalt/common/Types.h>
#include <basalt/math/Vec3.h>

namespace basalt {
namespace gfx {
namespace backend {

enum class VertexElementUsage : i8 {
  POSITION,
  POSITION_TRANSFORMED,
  NORMAL,
  COLOR_DIFFUSE,
  COLOR_SPECULAR,
  TEXTURE_COORDS
};

enum class VertexElementType : i8 {
  F32_1,
  F32_2,
  F32_3,
  F32_4,
  U32_1
};

struct VertexElement final {
  VertexElementUsage usage;
  VertexElementType type;
};

class VertexLayout final {
public:
  std::vector<VertexElement> m_elements;
};

enum class PrimitiveType : i8 {
  POINT_LIST,
  LINE_LIST,
  LINE_STRIP,
  TRIANGLE_LIST,
  TRIANGLE_STRIP,
  TRIANGLE_FAN
};

struct VertexData final {
  VertexLayout layout;
  i32 numVertices;
  PrimitiveType primitiveType;

  void* data;
};

using MeshHandle = Handle<HandleTarget::GFX_MESH>;

using TextureHandle = Handle<HandleTarget::GFX_TEXTURE>;

} // namespace backend
} // namespace gfx
} // namespace basalt

#endif // !BS_GFX_BACKEND_VERTEX_BUFFER_H
