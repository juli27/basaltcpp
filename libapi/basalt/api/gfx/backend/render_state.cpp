#include <basalt/api/gfx/backend/render_state.h>

namespace basalt::gfx {

auto RenderState::fill_mode(const FillMode mode) -> RenderState {
  return RenderState {RenderStateType::FillMode, mode};
}

auto RenderState::shade_mode(const ShadeMode mode) -> RenderState {
  return RenderState {RenderStateType::ShadeMode, mode};
}

auto RenderState::type() const noexcept -> RenderStateType {
  return mType;
}

auto RenderState::value() const noexcept -> Value {
  return mValue;
}

} // namespace basalt::gfx
