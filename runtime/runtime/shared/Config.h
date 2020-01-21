#pragma once
#ifndef BASALT_SHARED_CONFIG_H
#define BASALT_SHARED_CONFIG_H

#include "Types.h"

#include <string>

namespace basalt {

enum class WindowMode : u8 {
  Windowed,
  Fullscreen,
  FullscreenExclusive
};


enum class GfxBackendApi : u8 {
  Default,
  D3D9
};


struct Config final {
  std::string mAppName = "Basalt App";
  i32 mWindowWidth = 0; // 0 == don't care
  i32 mWindowHeight = 0; // 0 == don't care
  WindowMode mWindowMode = WindowMode::Windowed;
  bool mIsWindowResizeable = true;
  GfxBackendApi mGfxBackendApi = GfxBackendApi::Default;
};

} // namespace basalt

#endif // !BASALT_SHARED_CONFIG_H
