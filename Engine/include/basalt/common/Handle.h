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
  constexpr ValueT GetValue() const noexcept;

public:
  constexpr bool operator==(const HandleBase& rhs) const noexcept;
  constexpr bool operator!=(const HandleBase& rhs) const noexcept;

  inline HandleBase& operator=(const HandleBase&) noexcept = default;
  inline HandleBase& operator=(HandleBase&&) noexcept = default;

public:
  constexpr explicit operator bool() const noexcept;

private:
  ValueT mValue;

public:
  static constexpr ValueT sInvalidValue = std::numeric_limits<ValueT>::max();
};


constexpr HandleBase::HandleBase() noexcept : mValue(sInvalidValue) {}


constexpr HandleBase::HandleBase(ValueT value) noexcept : mValue(value) {
  // TODO: assert (value != sInvalidValue)
}


constexpr void HandleBase::Invalidate() noexcept {
  mValue = sInvalidValue;
}


constexpr HandleBase::ValueT HandleBase::GetValue() const noexcept {
  return mValue;
}


constexpr bool HandleBase::operator==(const HandleBase& rhs) const noexcept {
  return mValue == rhs.mValue;
}


constexpr bool HandleBase::operator!=(const HandleBase& rhs) const noexcept {
  return !(*this == rhs);
}


constexpr HandleBase::operator bool() const noexcept {
  return mValue != sInvalidValue;
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
