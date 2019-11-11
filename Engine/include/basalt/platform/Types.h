#pragma once
#ifndef BASALT_PLATFORM_TYPES_H
#define BASALT_PLATFORM_TYPES_H

#include <basalt/math/Vec2.h>
#include <basalt/shared/Types.h>

#include <string>

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
  WindowMode mMode = WindowMode::Windowed;
  bool mResizeable = false;
};

} // namespace basalt

#endif // !BASALT_PLATFORM_TYPES_H
