#include <basalt/gfx/utils.h>

#include <basalt/api/shared/asserts.h>

namespace basalt::gfx {

auto to_string(const ImageFormat format) noexcept -> const char* {
  switch (format) {
  case ImageFormat::Unknown:
    return "Unknown";
  case ImageFormat::B5G6R5:
    return "B5G6R5";
  case ImageFormat::B5G5R5X1:
    return "B5G5R5X1";
  case ImageFormat::B5G5R5A1:
    return "B5G5R5A1";
  case ImageFormat::B8G8R8X8:
    return "B8G8R8X8";
  case ImageFormat::B8G8R8A8:
    return "B8G8R8A8";
  case ImageFormat::B10G10R10A2:
    return "B10G10R10A2";
  case ImageFormat::D16:
    return "D16";
  case ImageFormat::D24X8:
    return "D24X8";
  case ImageFormat::D24S8:
    return "D24S8";
  }

  BASALT_CRASH("unhandled format");
}

} // namespace basalt::gfx
