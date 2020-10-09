#include "utils.h"

#include <fmt/core.h>

namespace basalt::gfx {

auto to_string(const SurfaceFormat format) noexcept -> const char* {
  switch (format) {
  case SurfaceFormat::B5G6R5:
    return "B5G6R5";

  case SurfaceFormat::B5G5R5X1:
    return "B5G5R5X1";

  case SurfaceFormat::B5G5R5A1:
    return "B5G5R5A1";

  case SurfaceFormat::B8G8R8X8:
    return "B8G8R8X8";

  case SurfaceFormat::B8G8R8A8:
    return "B8G8R8A8";

  case SurfaceFormat::B10G10R10A2:
    return "B10G10R10A2";
  }

  return "(unknown format)";
}

auto to_string(const AdapterMode& adapterMode) noexcept -> std::string {
  return fmt::format("{}x{} {}Hz {}", adapterMode.width, adapterMode.height,
                     adapterMode.refreshRate,
                     to_string(adapterMode.displayFormat));
}

} // namespace basalt::gfx
