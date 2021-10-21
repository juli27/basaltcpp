#include <basalt/api/gfx/backend/render_state.h>

namespace basalt::gfx {

auto RenderState::ambient(const Color& ambientColor) -> RenderState {
  return RenderState {RenderStateType::Ambient, ambientColor};
}

auto RenderState::fill_mode(const FillMode mode) -> RenderState {
  return RenderState {RenderStateType::FillMode, mode};
}

auto RenderState::depth_test(const DepthTestPass function) -> RenderState {
  return RenderState {RenderStateType::DepthTest, function};
}

auto RenderState::depth_write(const bool enabled) -> RenderState {
  return RenderState {RenderStateType::DepthWrite, enabled};
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
