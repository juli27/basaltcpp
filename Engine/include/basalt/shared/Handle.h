#pragma once
#ifndef BASALT_SHARED_HANDLE_H
#define BASALT_SHARED_HANDLE_H

#include "Types.h"

#include <limits>

namespace basalt {
namespace _internal {

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

  [[nodiscard]] constexpr auto get_value() const noexcept -> ValueT;

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

constexpr void HandleBase::invalidate() noexcept {
  mValue = INVALID_VALUE;
}

constexpr HandleBase::ValueT HandleBase::get_value() const noexcept {
  return mValue;
}

} // namespace _internal

template <typename Tag>
struct Handle final : _internal::HandleBase {
  // inherit base class constructors
  using HandleBase::HandleBase;
};

} // namespace basalt

#endif // !BASALT_SHARED_HANDLE_H
