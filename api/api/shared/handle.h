#pragma once

#include "asserts.h"

#include "api/base/types.h"

#include <limits>

namespace basalt {
namespace detail {

struct HandleBase {
  using ValueType = u32;

  constexpr HandleBase() noexcept = default;

  constexpr explicit HandleBase(const ValueType value) noexcept
    : mValue {value} {
    BASALT_ASSERT_MSG(mValue != NULL_VALUE, "null handle value");
  }

  constexpr explicit operator bool() const noexcept {
    return !is_null();
  }

  constexpr auto operator==(const HandleBase& rhs) const noexcept -> bool {
    return mValue == rhs.mValue;
  }

  constexpr auto operator!=(const HandleBase& rhs) const noexcept -> bool {
    return !(*this == rhs);
  }

  [[nodiscard]] constexpr auto is_null() const noexcept -> bool {
    return mValue == NULL_VALUE;
  }

  [[nodiscard]] constexpr auto value() const noexcept -> ValueType {
    return mValue;
  }

private:
  static constexpr ValueType NULL_VALUE {std::numeric_limits<ValueType>::max()};

  ValueType mValue {NULL_VALUE};
};

} // namespace detail

template <typename Tag>
struct Handle final : detail::HandleBase {
  // inherit base class constructors
  using HandleBase::HandleBase;

  static constexpr auto null() noexcept -> Handle {
    return Handle {};
  }
};

} // namespace basalt
