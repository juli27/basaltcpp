#include "vec2.h"

namespace basalt {

template <typename T>
void Vec2<T>::set(const T ax, const T ay) noexcept {
  x = ax;
  y = ay;
}

template struct Vec2<i32>;

} // basalt
