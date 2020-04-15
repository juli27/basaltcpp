#include "runtime/math/Vec2.h"

#include "runtime/shared/Types.h"

namespace basalt::math {

template struct Vec2<i32>;
template struct Vec2<f32>;

template<typename T>
void Vec2<T>::set(T x, T y) noexcept {
  mX = x;
  mY = y;
}

template void Vec2<i32>::set(i32 x, i32 y) noexcept;
template void Vec2<f32>::set(f32 x, f32 y) noexcept;

template<typename T>
void Vec2<T>::set_x(T x) noexcept {
  mX = x;
}

template void Vec2<i32>::set_x(i32 x) noexcept;
template void Vec2<f32>::set_x(f32 x) noexcept;

template<typename T>
void Vec2<T>::set_y(T y) noexcept {
  mY = y;
}

template void Vec2<i32>::set_y(i32 y) noexcept;
template void Vec2<f32>::set_y(f32 y) noexcept;

} // basalt::math
