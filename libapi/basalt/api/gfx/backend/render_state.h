#pragma once

#include <basalt/api/gfx/backend/types.h>

#include <variant>

namespace basalt::gfx {

struct RenderState {
  using Value = RenderStateValue;

  constexpr RenderState(const RenderState&) = default;
  constexpr RenderState(RenderState&&) noexcept = default;

  ~RenderState() = default;

  constexpr auto operator=(const RenderState&) -> RenderState& = delete;
  constexpr auto operator=(RenderState&&) noexcept -> RenderState& = delete;

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
