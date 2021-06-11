#pragma once

#include <basalt/gfx/backend/d3d9/d3d9_custom.h>

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/color.h>

#include <basalt/api/base/utils.h>

#if BASALT_DEBUG_BUILD

// TODO: add detailed logging
#define D3D9CALL(x) BASALT_ASSERT(SUCCEEDED((x)), "Direct3D 9 call failed: " #x)

#else // BASALT_DEBUG_BUILD

#define D3D9CALL(x) (x)

#endif // !BASALT_DEBUG_BUILD

#if BASALT_DEV_BUILD

#define PIX_BEGIN_EVENT(color, name) D3DPERF_BeginEvent(color, name)
#define PIX_END_EVENT() D3DPERF_EndEvent()
#define PIX_SET_MARKER(color, name) D3DPERF_SetMarker(color, name)

#else // BASALT_DEV_BUILD

#define PIX_BEGIN_EVENT(color, name)
#define PIX_END_EVENT()
#define PIX_SET_MARKER(color, name)

#endif // !BASALT_DEV_BUILD

namespace basalt::gfx {

constexpr auto to_d3d_color(const Color& color) noexcept -> D3DCOLOR {
  return enum_cast(color.to_argb());
}

} // namespace basalt::gfx
