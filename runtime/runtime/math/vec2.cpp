#include "vec2.h"

namespace basalt {

template struct Vec2<i32>;
template struct Vec2<f32>;

template <typename T>
void Vec2<T>::set(const T ax, const T ay) noexcept {
  x = ax;
  y = ay;
}

template void Vec2<i32>::set(i32 ax, i32 ay) noexcept;
template void Vec2<f32>::set(f32 ax, f32 ay) noexcept;

} // basalt
