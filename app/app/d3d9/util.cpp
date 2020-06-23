#include "util.h"

using std::string_view;
using namespace std::literals;

namespace basalt::gfx {

auto to_string(const SurfaceFormat format) noexcept -> std::string_view {
  switch (format) {
  case SurfaceFormat::B5G6R5:
    return "B5G6R5"sv;

  case SurfaceFormat::B5G5R5X1:
    return "B5G5R5X1"sv;

  case SurfaceFormat::B5G5R5A1:
    return "B5G5R5A1"sv;

  case SurfaceFormat::B8G8R8X8:
    return "B8G8R8X8"sv;

  case SurfaceFormat::B8G8R8A8:
    return "B8G8R8A8"sv;

  case SurfaceFormat::B10G10R10A2:
    return "B10G10R10A2"sv;
  }

  return "(unknown format)"sv;
}

} // namespace basalt::gfx
