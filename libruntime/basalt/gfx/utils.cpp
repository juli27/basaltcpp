#include <basalt/gfx/utils.h>

#include <basalt/api/shared/asserts.h>

#include <fmt/core.h>

#include <numeric>

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
  }

  BASALT_CRASH("unhandled format");
}

auto to_string(const AdapterMode& mode) noexcept -> std::string {
  const auto gcd {std::gcd(mode.width, mode.height)};
  return fmt::format("{}x{} {}Hz ({}:{})", mode.width, mode.height,
                     mode.refreshRate, mode.width / gcd, mode.height / gcd);
}

} // namespace basalt::gfx
