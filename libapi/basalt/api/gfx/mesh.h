#pragma once

#include "backend/types.h"

#include <basalt/api/base/types.h>

#include <gsl/span>

#include <cstddef>

namespace basalt::gfx {

struct MeshCreateInfo {
  gsl::span<std::byte const> vertexData;
  u32 vertexCount{};
  VertexLayoutSpan layout;
  gsl::span<std::byte const> indexData;
  u32 indexCount{};
  IndexType indexType{IndexType::U16};
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
