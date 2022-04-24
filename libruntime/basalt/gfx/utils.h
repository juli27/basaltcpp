#pragma once

#include <basalt/gfx/types.h>

#include <basalt/api/gfx/types.h>

#include <string>

namespace basalt::gfx {

auto to_string(ImageFormat) noexcept -> const char*;
auto to_string(const AdapterMode&) noexcept -> std::string;

} // namespace basalt::gfx
