#pragma once

#include "d3d9_custom.h"
#include "types.h"

#include <runtime/shared/asserts.h>
#include <runtime/shared/color.h>
#include <runtime/shared/utils.h>

#include <string_view>

#ifdef BASALT_DEBUG_BUILD

// TODO: add detailed logging
#define D3D9CALL(x)                                                            \
  BASALT_ASSERT_MSG(SUCCEEDED((x)), "Direct3D 9 call failed: " #x)

#else // BASALT_DEBUG_BUILD

#define D3D9CALL(x) (x)

#endif // BASALT_DEBUG_BUILD

namespace basalt::gfx {

constexpr auto to_d3d_color(const Color& color) noexcept -> D3DCOLOR {
  return enum_cast(color.to_argb());
}

auto to_string(SurfaceFormat) noexcept -> std::string_view;

} // namespace basalt::gfx
