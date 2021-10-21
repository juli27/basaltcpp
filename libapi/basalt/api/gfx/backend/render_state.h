#pragma once

#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/shared/color.h>

#include <variant>

namespace basalt::gfx {

struct RenderState {
  using Value = RenderStateValue;

  constexpr RenderState(const RenderState&) = default;
  constexpr RenderState(RenderState&&) noexcept = default;

  ~RenderState() = default;

  constexpr auto operator=(const RenderState&) -> RenderState& = delete;
  constexpr auto operator=(RenderState&&) noexcept -> RenderState& = delete;

  [[nodiscard]] static auto ambient(const Color&) -> RenderState;
  [[nodiscard]] static auto fill_mode(FillMode) -> RenderState;
  [[nodiscard]] static auto depth_test(DepthTestPass) -> RenderState;
  [[nodiscard]] static auto depth_write(bool enabled) -> RenderState;
  [[nodiscard]] static auto shade_mode(ShadeMode) -> RenderState;

  [[nodiscard]] auto type() const noexcept -> RenderStateType;
  [[nodiscard]] auto value() const noexcept -> Value;

private:
  const RenderStateType mType;
  const Value mValue;

  constexpr RenderState(const RenderStateType type, Value value)
    : mType {type}, mValue {value} {
  }
};

} // namespace basalt::gfx
