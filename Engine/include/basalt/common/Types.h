#pragma once
#ifndef BS_COMMON_TYPES_H
#define BS_COMMON_TYPES_H

#include <cstdint>

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
 * \brief 32 bit floating point type.
 */
using f32 = float;

/**
 * \brief 64 bit floating point type.
 */
using f64 = double;

} // namespace basalt

#endif // !BS_COMMON_TYPES_H
