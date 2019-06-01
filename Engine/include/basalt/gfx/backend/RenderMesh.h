#pragma once
#ifndef BS_GFX_BACKEND_VERTEX_BUFFER_H
#define BS_GFX_BACKEND_VERTEX_BUFFER_H

#include <optional>
#include <vector>

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


struct RenderMeshHandle {
public:
  using IndexT = i16;
  using GenT = i8;
  using ValueT = i32;

  static_assert(sizeof(IndexT) + sizeof(GenT) <= sizeof(ValueT));

public:
  constexpr RenderMeshHandle();
  constexpr RenderMeshHandle(IndexT index, GenT gen);
  constexpr RenderMeshHandle(const RenderMeshHandle&) = default;
  constexpr RenderMeshHandle(RenderMeshHandle&&) = default;
  inline ~RenderMeshHandle() = default;

  constexpr bool IsValid() const;
  constexpr IndexT GetIndex() const;
  constexpr GenT GetGen() const;

  inline RenderMeshHandle& operator=(const RenderMeshHandle&) = default;
  inline RenderMeshHandle& operator=(RenderMeshHandle&&) = default;

private:
  ValueT m_value;
};


constexpr RenderMeshHandle::RenderMeshHandle() : m_value(0xFFFFFFFF) {}


constexpr RenderMeshHandle::RenderMeshHandle(IndexT index, GenT gen) : m_value(index) {
  m_value |= (static_cast<i32>(gen) << 16);
}


constexpr bool RenderMeshHandle::IsValid() const {
  return !(0x80000000 & m_value);
}


constexpr RenderMeshHandle::IndexT RenderMeshHandle::GetIndex() const {
  return static_cast<IndexT> (m_value);
}


constexpr RenderMeshHandle::GenT RenderMeshHandle::GetGen() const {
  return static_cast<GenT>(m_value >> 16);
}

} // namespace backend
} // namespace gfx
} // namespace basalt

#endif // !BS_GFX_BACKEND_VERTEX_BUFFER_H
