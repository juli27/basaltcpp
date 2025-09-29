#include <basalt/api/gfx/mesh.h>

namespace basalt::gfx {

Mesh::Mesh(VertexBufferHandle const vertexBuffer, u32 const startVertex,
           u32 const vertexCount, IndexBufferHandle const ibHandle,
           u32 const indexCount)
  : mVertexBuffer{vertexBuffer}
  , mStartVertex{startVertex}
  , mVertexCount{vertexCount}
  , mIndexBuffer{ibHandle}
  , mIndexCount{indexCount} {
}

auto Mesh::vertexBuffer() const -> VertexBufferHandle {
  return mVertexBuffer;
}

auto Mesh::startVertex() const -> u32 {
  return mStartVertex;
}

auto Mesh::vertexCount() const -> u32 {
  return mVertexCount;
}

auto Mesh::indexBuffer() const -> IndexBufferHandle {
  return mIndexBuffer;
}

auto Mesh::indexCount() const -> u32 {
  return mIndexCount;
}

} // namespace basalt::gfx
