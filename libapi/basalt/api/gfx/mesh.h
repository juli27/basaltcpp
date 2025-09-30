#pragma once

#include "backend/types.h"

#include <basalt/api/base/types.h>

#include <gsl/span>

#include <cstddef>

namespace basalt::gfx {

struct MeshCreateInfo {
  gfx::VertexBufferHandle vertexBuffer;
  u32 vertexCount{};
  gfx::IndexBufferHandle indexBuffer;
  u32 indexCount{};
};

class Mesh {
public:
  Mesh(VertexBufferHandle, u32 startVertex, u32 vertexCount, IndexBufferHandle,
       u32 indexCount);

  [[nodiscard]]
  auto vertexBuffer() const -> VertexBufferHandle;
  [[nodiscard]]
  auto startVertex() const -> u32;
  [[nodiscard]]
  auto vertexCount() const -> u32;
  [[nodiscard]]
  auto indexBuffer() const -> IndexBufferHandle;
  [[nodiscard]]
  auto indexCount() const -> u32;

private:
  VertexBufferHandle mVertexBuffer;
  u32 mStartVertex{};
  u32 mVertexCount{};
  IndexBufferHandle mIndexBuffer;
  u32 mIndexCount{};
};

} // namespace basalt::gfx
