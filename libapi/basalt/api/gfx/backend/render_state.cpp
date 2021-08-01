#include <basalt/api/gfx/backend/render_state.h>

namespace basalt::gfx {

auto RenderState::cull_mode(const CullMode mode) -> RenderState {
  return RenderState {RenderStateType::CullMode, mode};
}

auto RenderState::ambient(const Color& ambientColor) -> RenderState {
  return RenderState {RenderStateType::Ambient, ambientColor};
}

auto RenderState::lighting(const bool enabled) -> RenderState {
  return RenderState {RenderStateType::Lighting, enabled};
}

auto RenderState::type() const noexcept -> RenderStateType {
  return mType;
}

auto RenderState::value() const noexcept -> Value {
  return mValue;
}

} // namespace basalt::gfx
