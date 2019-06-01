#pragma once
#ifndef BS_GFX_BACKEND_D3D9_UTIL_H
#define BS_GFX_BACKEND_D3D9_UTIL_H

#include <basalt/common/Asserts.h>

#include "D3D9Header.h"

#ifdef BS_DEBUG_BUILD

// TODO: add detailed logging
#define D3D9CALL(x) BS_ASSERT(SUCCEEDED((x)), "Direct3D 9 call failed: " #x)

#else

#define D3D9CALL(x) (x)

#endif

#endif  // !BS_GFX_BACKEND_D3D9_UTIL_H
