#pragma once

#include <basalt/api/base/types.h>
#include <basalt/api/base/utils.h>

#include <array>
#include <cstddef>
#include <initializer_list>
#include <type_traits>
#include <utility>

namespace basalt {

// The enum *must not* have values larger than Size
// The enum *should* be contiguous (0, 1, 2,...)
template <typename Enum, typename T, uSize Size>
struct EnumArray {
private:
  using StorageType = std::array<T, Size>;

public:
  static_assert(std::is_enum_v<Enum>);
  static_assert(std::is_unsigned_v<std::underlying_type_t<Enum>>);

  using value_type = T;
  using reference = value_type&;
  using const_reference = value_type const&;
  using iterator = typename StorageType::iterator;
  using const_iterator = typename StorageType::const_iterator;
  using difference_type = std::ptrdiff_t;
  using size_type = uSize;

  constexpr EnumArray() noexcept = default;

  constexpr explicit EnumArray(
    std::initializer_list<std::pair<Enum const, T>> const init) {
    for (auto& [e, v] : init) {
      mStorage[enum_cast(e)] = std::move(v);
    }
  }

  [[nodiscard]]
  constexpr auto size() const noexcept -> uSize {
    return Size;
  }

  [[nodiscard]]
  constexpr auto
  operator[](Enum const e) const noexcept -> T const& {
    return mStorage[enum_cast(e)];
  }

  [[nodiscard]]
  constexpr auto
  operator[](Enum const e) noexcept -> T& {
    return mStorage[enum_cast(e)];
  }

  auto begin() noexcept -> iterator {
    return mStorage.begin();
  }

  auto begin() const noexcept -> const_iterator {
    return mStorage.begin();
  }

  auto end() noexcept -> iterator {
    return mStorage.end();
  }

  auto end() const noexcept -> const_iterator {
    return mStorage.end();
  }

  auto cbegin() const noexcept -> const_iterator {
    return mStorage.begin();
  }

  auto cend() const noexcept -> const_iterator {
    return mStorage.end();
  }

  auto data() noexcept -> T* {
    return mStorage.data();
  }

  auto data() const noexcept -> T const* {
    return mStorage.data();
  }

private:
  StorageType mStorage{};
};

} // namespace basalt
