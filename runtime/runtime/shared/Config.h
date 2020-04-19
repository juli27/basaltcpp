#pragma once
#ifndef BASALT_SHARED_CONFIG_H
#define BASALT_SHARED_CONFIG_H

#include "runtime/shared/Size2D.h"
#include "runtime/shared/Types.h"

#include <string>

namespace basalt {

enum class WindowMode : u8 {
  Windowed,
  Fullscreen,
  FullscreenExclusive,
};


enum class GfxBackendApi : u8 {
  Default,
  D3D9,
};


struct Config final {
  std::string appName {"Basalt App"};

  // the window size is only used for the windowed window mode
  Size2Du16 windowSize = Size2Du16::dont_care();

  WindowMode windowMode {WindowMode::Windowed};
  bool isWindowResizeable {true};
  GfxBackendApi gfxBackendApi {GfxBackendApi::Default};
};

} // namespace basalt

#endif // !BASALT_SHARED_CONFIG_H