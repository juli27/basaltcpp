#include <basalt/api/gfx/mesh.h>

namespace basalt::gfx {

Mesh::Mesh(VertexBufferHandle const vertexBuffer, u32 const vertexStart,
           u32 const vertexCount, IndexBufferHandle const ibHandle,
           u32 const indexCount)
  : mVertexBuffer{vertexBuffer}
  , mVertexStart{vertexStart}
  , mVertexCount{vertexCount}
  , mIndexBuffer{ibHandle}
  , mIndexCount{indexCount} {
}

auto Mesh::vertexBuffer() const -> VertexBufferHandle {
  return mVertexBuffer;
}

auto Mesh::vertexStart() const -> u32 {
  return mVertexStart;
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
