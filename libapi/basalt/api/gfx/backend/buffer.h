#pragma once

#include "types.h"
#include "vertex_layout.h"

namespace basalt::gfx {

struct VertexBufferCreateInfo {
  uDeviceSize sizeInBytes{};
  VertexLayoutSpan layout;
};

struct IndexBufferCreateInfo {
  uDeviceSize sizeInBytes{};
  IndexType type{IndexType::U16};
};

} // namespace basalt::gfx
