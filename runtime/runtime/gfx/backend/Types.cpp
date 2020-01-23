#include "runtime/gfx/backend/Types.h"

namespace basalt::gfx::backend {

VertexElement::VertexElement(const VertexElementType type
                           , const VertexElementUsage usage) noexcept
  : mUsage(usage), mType(type) {}

} // namespace basalt::gfx::backend
