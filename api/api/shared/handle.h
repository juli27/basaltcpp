#pragma once

#include "asserts.h"
#include "types.h"

#include <limits>

namespace basalt {
namespace detail {

struct HandleBase {
  using ValueT = u32;

  constexpr HandleBase() noexcept = default;

  constexpr explicit HandleBase(const ValueT value) noexcept : mValue {value} {
    BASALT_ASSERT_MSG(mValue != INVALID_VALUE, "invalid handle value");
  }

  constexpr explicit operator bool() const noexcept {
    return mValue != INVALID_VALUE;
  }

  constexpr auto operator==(const HandleBase& rhs) const noexcept -> bool {
    return mValue == rhs.mValue;
  }

  constexpr auto operator!=(const HandleBase& rhs) const noexcept -> bool {
    return !(*this == rhs);
  }

  constexpr void invalidate() noexcept {
    mValue = INVALID_VALUE;
  }

  [[nodiscard]] constexpr auto value() const noexcept -> ValueT {
    return mValue;
  }

private:
  static constexpr ValueT INVALID_VALUE {std::numeric_limits<ValueT>::max()};

  ValueT mValue {INVALID_VALUE};
};

} // namespace detail

template <typename Tag>
struct Handle final : detail::HandleBase {
  // inherit base class constructors
  using HandleBase::HandleBase;

  static constexpr auto invalid() -> Handle {
    return Handle {};
  }
};

} // namespace basalt
