#pragma once
#ifndef BASALT_RUNTIME_GFX_BACKEND_D3D9_UTIL_H
#define BASALT_RUNTIME_GFX_BACKEND_D3D9_UTIL_H

#include "runtime/gfx/backend/d3d9/d3d9_custom.h"
#include "runtime/shared/Asserts.h"

#ifdef BASALT_DEBUG_BUILD

// TODO: add detailed logging
#define D3D9CALL(x) BASALT_ASSERT_MSG(SUCCEEDED((x)), "Direct3D 9 call failed: " #x)

#else // BASALT_DEBUG_BUILD

#define D3D9CALL(x) (x)

#endif // BASALT_DEBUG_BUILD

#endif  // !BASALT_RUNTIME_GFX_BACKEND_D3D9_UTIL_H
