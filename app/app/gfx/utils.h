#pragma once

#include "types.h"

#include <string>

namespace basalt::gfx {

auto to_string(SurfaceFormat) noexcept -> const char*;
auto to_string(const AdapterMode&) noexcept -> std::string;

} // namespace basalt::gfx
