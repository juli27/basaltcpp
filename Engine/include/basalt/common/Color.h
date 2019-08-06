#pragma once
#ifndef BS_COMMON_COLOR_H
#define BS_COMMON_COLOR_H

#include "Types.h"

namespace basalt {


class Color final {
public:
  constexpr Color();

  constexpr Color(u8 r, u8 g, u8 b);

  constexpr Color(u8 r, u8 g, u8 b, u8 a);

  // ARGB word-order
  constexpr u32 ToARGB() const;

  constexpr u8 GetRed() const;

  constexpr u8 GetGreen() const;

  constexpr u8 GetBlue() const;

  constexpr u8 GetAlpha() const;

private:
  u8 m_r;
  u8 m_g;
  u8 m_b;
  u8 m_a;

public:
  // ARGB word order
  static constexpr Color FromARGB(u32 argb);
};


constexpr Color::Color() : Color(0, 0, 0, 0) {}


constexpr Color::Color(u8 r, u8 g, u8 b) : Color(r, g, b, 255) {}


constexpr Color::Color(u8 r, u8 g, u8 b, u8 a)
  : m_r(r)
  , m_g(g)
  , m_b(b)
  , m_a(a) {}


constexpr u32 Color::ToARGB() const {
  u32 color = m_a;
  color <<= 8;
  color |= m_r;
  color <<= 8;
  color |= m_g;
  color <<= 8;
  color |= m_b;

  return color;
}


constexpr u8 Color::GetRed() const {
  return m_r;
}


constexpr u8 Color::GetGreen() const {
  return m_g;
}


constexpr u8 Color::GetBlue() const {
  return m_b;
}


constexpr u8 Color::GetAlpha() const {
  return m_a;
}


constexpr Color Color::FromARGB(u32 argb) {
  u8 a = static_cast<u8>(argb);
  u8 r = static_cast<u8>(argb >> 8);
  u8 g = static_cast<u8>(argb >> 16);
  u8 b = static_cast<u8>(argb >> 24);

  return Color(r, g, b, a);
}

} // namespace basalt

#endif // !BS_COMMON_COLOR_H
