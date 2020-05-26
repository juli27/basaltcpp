#pragma once
#ifndef BASALT_APP_D3D9_UTIL_H
#define BASALT_APP_D3D9_UTIL_H

#include "d3d9_custom.h"
#include "types.h"

#include "runtime/shared/Asserts.h"

#include <string_view>

#ifdef BASALT_DEBUG_BUILD

// TODO: add detailed logging
#define D3D9CALL(x) BASALT_ASSERT_MSG(SUCCEEDED((x)), "Direct3D 9 call failed: " #x)

#else // BASALT_DEBUG_BUILD

#define D3D9CALL(x) (x)

#endif // BASALT_DEBUG_BUILD

namespace basalt::gfx::backend {

auto to_string(SurfaceFormat) -> std::string_view;

} // namespace basalt::gfx::backend

#endif  // !BASALT_APP_D3D9_UTIL_H
