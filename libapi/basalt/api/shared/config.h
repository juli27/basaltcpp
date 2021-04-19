#pragma once

#include <basalt/api/shared/size2d.h>
#include <basalt/api/shared/types.h>

#include <basalt/api/base/types.h>

#include <string>

namespace basalt {

using namespace std::string_literals;

enum class GfxBackendApi : u8 {
  Default,
  Direct3D9,
};

struct Config final {
  std::string appName {"Basalt App"s};

  // the window size is only used for the windowed window mode
  Size2Du16 preferredSurfaceSize {Size2Du16::dont_care()};

  WindowMode windowMode {WindowMode::Windowed};
  bool isWindowResizeable {true};
  GfxBackendApi gfxBackendApi {GfxBackendApi::Default};
  bool debugUiEnabled {false};

  static auto defaults() -> Config {
    return Config {};
  }
};

} // namespace basalt
