#pragma once

#include <basalt/gfx/backend/d3d9/d3d9_custom.h>

#include <basalt/api/shared/color.h>

#include <basalt/api/base/utils.h>

namespace basalt::gfx {

constexpr auto to_d3d_color(const Color& color) noexcept -> D3DCOLOR {
  return enum_cast(color.to_argb());
}

} // namespace basalt::gfx
