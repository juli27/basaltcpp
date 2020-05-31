#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace basalt {

/**
 * \brief 8 bit signed integer type.
 */
using i8 = std::int8_t;

/**
 * \brief 16 bit signed integer type.
 */
using i16 = std::int16_t;

/**
 * \brief 32 bit signed integer type.
 */
using i32 = std::int32_t;

/**
 * \brief 64 bit signed integer type.
 */
using i64 = std::int64_t;

/**
 * \brief 8 bit unsigned integer type.
 */
using u8 = std::uint8_t;

/**
 * \brief 16 bit unsigned integer type.
 */
using u16 = std::uint16_t;

/**
 * \brief 32 bit unsigned integer type.
 */
using u32 = std::uint32_t;

/**
 * \brief 64 bit unsigned integer type.
 */
using u64 = std::uint64_t;

/**
 * \brief Address sized unsigned integer type.
 */
using uSize = std::size_t;

/**
 * \brief 32 bit floating point type.
 */
using f32 = float;

/**
 * \brief 64 bit floating point type.
 */
using f64 = double;

template <typename E, std::enable_if_t<std::is_enum_v<E>, int> = 0>
constexpr auto enum_cast(const E e) noexcept {
  return static_cast<std::underlying_type_t<E>>(e);
}

} // namespace basalt
