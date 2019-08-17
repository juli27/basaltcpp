#pragma once
#ifndef BS_COMMON_HANDLE_H
#define BS_COMMON_HANDLE_H

#include <limits>

#include "Types.h"

namespace basalt {
namespace _internal {


class HandleBase {
public:
  using ValueT = u32;

  constexpr HandleBase() noexcept = default;
  constexpr explicit HandleBase(ValueT value) noexcept;
  constexpr HandleBase(const HandleBase&) noexcept = default;
  constexpr HandleBase(HandleBase&&) noexcept = default;
  inline ~HandleBase() noexcept = default;

  constexpr explicit operator bool() const noexcept;

  inline auto operator=(const HandleBase&) noexcept -> HandleBase& = default;
  inline auto operator=(HandleBase&&) noexcept -> HandleBase& = default;

  constexpr auto operator==(const HandleBase& rhs) const noexcept -> bool;
  constexpr auto operator!=(const HandleBase& rhs) const noexcept -> bool;

  constexpr void Invalidate() noexcept;

  [[nodiscard]] constexpr auto GetValue() const noexcept -> ValueT;

private:
  static constexpr ValueT INVALID_VALUE = std::numeric_limits<ValueT>::max();

  ValueT mValue = INVALID_VALUE;
};


constexpr HandleBase::HandleBase(const ValueT value) noexcept : mValue(value) {
  // TODO: assert (value != INVALID_VALUE)
  // can't be done yet because of the Asserts.h -> Log.h -> Windows.h dependency
  // in yet another header file
}


constexpr HandleBase::operator bool() const noexcept {
  return mValue != INVALID_VALUE;
}


constexpr auto HandleBase::operator==(const HandleBase& rhs) const noexcept
-> bool {
  return mValue == rhs.mValue;
}


constexpr auto HandleBase::operator!=(const HandleBase& rhs) const noexcept
-> bool {
  return !(*this == rhs);
}


constexpr void HandleBase::Invalidate() noexcept {
  mValue = INVALID_VALUE;
}


constexpr HandleBase::ValueT HandleBase::GetValue() const noexcept {
  return mValue;
}

} // namespace _internal


template <typename Tag>
class Handle final : public _internal::HandleBase {
public:
  // inherit base class constructors
  using HandleBase::HandleBase;
};

} // namespace basalt

#endif // !BS_COMMON_HANDLE_H
