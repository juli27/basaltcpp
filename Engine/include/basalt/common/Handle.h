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

public:
  constexpr HandleBase() noexcept;
  constexpr explicit HandleBase(ValueT value) noexcept;
  constexpr HandleBase(const HandleBase&) noexcept = default;
  constexpr HandleBase(HandleBase&&) noexcept = default;
  inline ~HandleBase() noexcept = default;

public:
  constexpr void Invalidate() noexcept;
  constexpr auto GetValue() const noexcept -> ValueT;

public:
  constexpr auto operator==(const HandleBase& rhs) const noexcept -> bool;
  constexpr auto operator!=(const HandleBase& rhs) const noexcept -> bool;

  inline auto operator=(const HandleBase&) noexcept -> HandleBase& = default;
  inline auto operator=(HandleBase&&) noexcept -> HandleBase& = default;

public:
  constexpr explicit operator bool() const noexcept;

private:
  ValueT mValue;

private:
  static constexpr ValueT INVALID_VALUE = std::numeric_limits<ValueT>::max();
};


constexpr HandleBase::HandleBase() noexcept : mValue(INVALID_VALUE) {}


constexpr HandleBase::HandleBase(const ValueT value) noexcept : mValue(value) {
  // TODO: assert (value != INVALID_VALUE)
}


constexpr void HandleBase::Invalidate() noexcept {
  mValue = INVALID_VALUE;
}


constexpr HandleBase::ValueT HandleBase::GetValue() const noexcept {
  return mValue;
}


constexpr auto HandleBase::operator==(const HandleBase& rhs) const noexcept -> bool {
  return mValue == rhs.mValue;
}


constexpr bool HandleBase::operator!=(const HandleBase& rhs) const noexcept {
  return !(*this == rhs);
}


constexpr HandleBase::operator bool() const noexcept {
  return mValue != INVALID_VALUE;
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
