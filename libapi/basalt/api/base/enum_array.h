#pragma once

#include <basalt/api/base/types.h>
#include <basalt/api/base/utils.h>

#include <array>
#include <initializer_list>
#include <type_traits>
#include <utility>

namespace basalt {

// The enum *must not* have values larger than Size
// The enum *must not* have any negative values
// The enum *should* be contiguous (0, 1, 2,...)
template <typename K, typename V, uSize Size>
struct EnumArray {
  static_assert(std::is_enum_v<K>);

  constexpr EnumArray() noexcept = default;

  constexpr explicit EnumArray(std::initializer_list<std::pair<K, V>> init) {
    for (auto& pair : init) {
      mStorage[enum_cast(pair.first)] = std::move(pair.second);
    }
  }

  [[nodiscard]] constexpr auto size() const noexcept -> uSize {
    return Size;
  }

  [[nodiscard]] constexpr auto operator[](const K key) const -> const V& {
    return mStorage[enum_cast(key)];
  }

  [[nodiscard]] constexpr auto operator[](const K key) -> V& {
    return mStorage[enum_cast(key)];
  }

private:
  std::array<V, Size> mStorage {};
};

// can not make it work in MSVC. Is MSVC not implementing CWG 1591?
// TODO: submit feedback
//template <class K, typename V, uSize N>
//EnumArray(std::pair<K, V>(&&)[N]) -> EnumArray<K, V, N>;

} // namespace basalt
