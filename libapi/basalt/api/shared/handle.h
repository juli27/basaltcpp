#pragma once

#include <basalt/api/shared/asserts.h>

#include <basalt/api/base/types.h>

#include <functional>
#include <limits>

namespace basalt {

struct NullHdl {
  struct Tag {};

  constexpr explicit NullHdl(Tag) {
  }
};

inline constexpr NullHdl nullhdl{NullHdl::Tag{}};

namespace detail {

struct HandleBase {
  using ValueType = u32;

  constexpr HandleBase() noexcept = default;

  constexpr HandleBase(NullHdl) noexcept {
  }

  constexpr explicit HandleBase(ValueType const value) noexcept
    : mValue{value} {
    BASALT_ASSERT(mValue != NULL_VALUE, "null handle value");
  }

  constexpr explicit operator bool() const noexcept {
    return !is_null();
  }

  constexpr auto operator==(HandleBase const& rhs) const noexcept -> bool {
    return mValue == rhs.mValue;
  }

  constexpr auto operator!=(HandleBase const& rhs) const noexcept -> bool {
    return !(*this == rhs);
  }

  [[nodiscard]] constexpr auto is_null() const noexcept -> bool {
    return mValue == NULL_VALUE;
  }

  [[nodiscard]] constexpr auto value() const noexcept -> ValueType {
    return mValue;
  }

private:
  static constexpr auto NULL_VALUE = std::numeric_limits<ValueType>::max();

  ValueType mValue{NULL_VALUE};
};

} // namespace detail

template <typename Tag>
struct Handle final : detail::HandleBase {
  // inherit base class constructors
  using HandleBase::HandleBase;
};

} // namespace basalt

template <typename T>
struct std::hash<basalt::Handle<T>> {
  auto operator()(basalt::Handle<T> const& handle) const -> std::size_t {
    return std::hash<typename basalt::Handle<T>::ValueType>{}(handle.value());
  }
};
