#pragma once

#include <basalt/api/base/types.h>
#include <basalt/api/base/utils.h>

#include <bitset>
#include <type_traits>

namespace basalt {

template <typename E, E MaxEnumValue>
struct EnumSet final {
  static_assert(std::is_enum_v<E>);
  static_assert(std::is_unsigned_v<std::underlying_type_t<E>>);

  constexpr EnumSet() noexcept = default;

  template <typename... Es>
  explicit constexpr EnumSet(E e, Es... es) noexcept : mBits{bits(e, es...)} {
  }

  constexpr auto operator&=(EnumSet const& rhs) -> EnumSet& {
    mBits &= rhs.mBits;

    return *this;
  }

  [[nodiscard]] constexpr auto operator[](E const e) const noexcept -> bool {
    return has(e);
  }

  [[nodiscard]]
  constexpr auto has(E const e) const noexcept -> bool {
    return is_in_range(e) && mBits[to_index(e)];
  }

  auto set(E const e) -> void {
    mBits.set(to_index(e));
  }

  auto unset(E const e) -> void {
    mBits.reset(to_index(e));
  }

private:
  static constexpr auto to_index(E const e) noexcept -> uSize {
    return enum_cast(e);
  }

  static constexpr auto is_in_range(E const e) noexcept -> bool {
    return e <= MaxEnumValue;
  }

  static constexpr auto to_bit(E const e) noexcept -> u64 {
    return 1ULL << to_index(e);
  }

  template <typename... Es>
  static constexpr auto bits(E const e, Es... es) noexcept -> u64 {
    return (to_bit(e) | ... | to_bit(es));
  }

  static constexpr auto sNumBits = to_index(MaxEnumValue) + 1;

  std::bitset<sNumBits> mBits;
};

} // namespace basalt
