#pragma once
#ifndef BS_PLATFORM_WINDOW_TYPES_H
#define BS_PLATFORM_WINDOW_TYPES_H

#include <string>

#include <basalt/common/Types.h>
#include <basalt/math/Vec2.h>

namespace basalt {

enum class WindowMode : i8 {
  Windowed,
  Fullscreen,
  FullscreenExclusive
};

struct WindowDesc {
  std::string mTitle;
  // TODO: will this be ignored with fullscreen? window size vs videomode / backbuffer size
  math::Vec2i32 mSize;
  WindowMode mMode{};
  bool mResizeable = false;
};

} // namespace basalt

#endif // !BS_PLATFORM_WINDOW_TYPES_H
