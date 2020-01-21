#pragma once
#ifndef BASALT_GFX_BACKEND_D3D9_UTIL_H
#define BASALT_GFX_BACKEND_D3D9_UTIL_H

#include "D3D9Header.h"

#include "runtime/shared/Asserts.h"

#ifdef BASALT_DEBUG_BUILD

// TODO: add detailed logging
#define D3D9CALL(x) BASALT_ASSERT(SUCCEEDED((x)), "Direct3D 9 call failed: " #x)

#else // BASALT_DEBUG_BUILD

#define D3D9CALL(x) (x)

#endif // BASALT_DEBUG_BUILD

#endif  // !BASALT_GFX_BACKEND_D3D9_UTIL_H
