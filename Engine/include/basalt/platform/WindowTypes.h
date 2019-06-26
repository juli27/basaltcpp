#pragma once
#ifndef BS_PLATFORM_WINDOW_TYPES_H
#define BS_PLATFORM_WINDOW_TYPES_H

#include <string>

#include <basalt/common/Types.h>
#include <basalt/math/Vec2.h>

namespace basalt {

enum class WindowMode : i8 {
  WINDOWED,
  FULLSCREEN,
  FULLSCREEN_BORDERLESS
};

struct WindowDesc {
  std::string title;
  // TODO: will this be ignored with fullscreen? window size vs videomode / backbuffer size
  math::Vec2i32 size;
  WindowMode mode{};
  bool resizeable{};
};

} // namespace basalt

#endif // !BS_PLATFORM_WINDOW_TYPES_H
