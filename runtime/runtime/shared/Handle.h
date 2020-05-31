#pragma once

#include "Asserts.h"
#include "Types.h"

#include <limits>

namespace basalt {
namespace detail {

struct HandleBase {
  using ValueT = u32;

  constexpr HandleBase() noexcept = default;
  constexpr explicit HandleBase(ValueT value) noexcept;
  constexpr HandleBase(const HandleBase&) noexcept = default;
  constexpr HandleBase(HandleBase&&) noexcept = default;
  ~HandleBase() noexcept = default;

  constexpr explicit operator bool() const noexcept;

  auto operator=(const HandleBase&) noexcept -> HandleBase& = default;
  auto operator=(HandleBase&&) noexcept -> HandleBase& = default;

  constexpr auto operator==(const HandleBase& rhs) const noexcept -> bool;
  constexpr auto operator!=(const HandleBase& rhs) const noexcept -> bool;

  constexpr void invalidate() noexcept;

  [[nodiscard]]
  constexpr auto value() const noexcept -> ValueT {
    return mValue;
  }

private:
  static constexpr ValueT INVALID_VALUE {std::numeric_limits<ValueT>::max()};

  ValueT mValue {INVALID_VALUE};
};


constexpr HandleBase::HandleBase(const ValueT value) noexcept
  : mValue {value} {
  BASALT_ASSERT_MSG(mValue != INVALID_VALUE, "invalid handle value");
}

constexpr HandleBase::operator bool() const noexcept {
  return mValue != INVALID_VALUE;
}

constexpr auto HandleBase::operator==(
  const HandleBase& rhs
) const noexcept -> bool {
  return mValue == rhs.mValue;
}

constexpr auto HandleBase::operator!=(
  const HandleBase& rhs
) const noexcept -> bool {
  return !(*this == rhs);
}

constexpr void HandleBase::invalidate() noexcept {
  mValue = INVALID_VALUE;
}

} // namespace detail

template <typename Tag>
struct Handle final : detail::HandleBase {
  // inherit base class constructors
  using HandleBase::HandleBase;
};

} // namespace basalt
