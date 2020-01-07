#include "Color.h"

namespace basalt {

auto Color::operator=(const Color&) noexcept -> Color& = default;

auto Color::operator=(Color&&) noexcept -> Color& = default;

}
