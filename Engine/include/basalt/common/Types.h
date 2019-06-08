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


using f32 = float;


using f64 = double;


enum class [[nodiscard]] Result : i32 {
  OK,

  ERR,
  ERR_INVALID_HANDLE
};


enum class Key : i32 {
  UNKNOWN = 0,
  F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
  ESCAPE,
  TAB,
  CAPS_LOCK, SHIFT_LEFT, SHIFT_RIGHT,
  ALT,
  SUPER_LEFT,
  SUPER_RIGHT,
  INSERT, DELETE, HOME, END, PAGE_UP, PAGE_DOWN,
  LEFT_ARROW, RIGHT_ARROW, UP_ARROW, DOWN_ARROW,
  NUM_0, NUM_1, NUM_2, NUM_3, NUM_4,
  NUM_5, NUM_6, NUM_7, NUM_8, NUM_9,
  NUM_ADD, NUM_SUB, NUM_MUL, NUM_DIV,
  NUM_DECIMAL, NUM_LOCK, NUM_ENTER,
  ROW_0, ROW_1, ROW_2, ROW_3, ROW_4,
  ROW_5, ROW_6, ROW_7, ROW_8, ROW_9,
  BACKSPACE, SPACE, ENTER, MENU,
  SCROLL_LOCK, PRINT, PLUS,
  A, B, C, D, E, F, G, H, I, J, K, L, M,
  N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
  MINUS, PAUSE,
  COMMA, PERIOD,
  CONTROL_LEFT, CONTROL_RIGHT,
  OEM_1, OEM_2, OEM_3, OEM_4, OEM_5, OEM_6, OEM_7, OEM_8, OEM_9,

  NUMBER_OF_KEYS
};


enum class MouseButton : i32 {
  UNKNOWN = 0,
  LEFT, RIGHT, MIDDLE, BUTTON4, BUTTON5,

  NUMBER_OF_BUTTONS
};

} // namespace basalt

#endif // !BS_COMMON_TYPES_H
