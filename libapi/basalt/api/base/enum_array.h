#pragma once

#include <basalt/api/base/types.h>
#include <basalt/api/base/utils.h>

#include <array>
#include <type_traits>

namespace basalt {

// The enum *must not* have values larger than Size
// The enum *must not* have any negative values
// The enum *should* be contiguous (0, 1, 2,...)
template <typename K, typename V, u64 Size>
struct EnumArray {
  static_assert(std::is_enum_v<K>);

  constexpr EnumArray() noexcept = default;

  auto operator[](const K key) const -> const V& {
    return mStorage[enum_cast(key)];
  }

  auto operator[](const K key) -> V& {
    return mStorage[enum_cast(key)];
  }

private:
  std::array<V, Size> mStorage {};
};

} // namespace basalt
