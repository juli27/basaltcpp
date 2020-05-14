#include "runtime/gfx/backend/RenderCommand.h"

namespace basalt::gfx::backend {

auto RenderCommandBuffer::ambient_light() const -> const Color& {
  return mAmbientLightColor;
}

void RenderCommandBuffer::set_ambient_light(const Color& color) {
  mAmbientLightColor = color;
}

} // namespace basalt::gfx::backend
