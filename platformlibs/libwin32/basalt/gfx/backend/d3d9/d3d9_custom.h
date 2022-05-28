#pragma once

#include <basalt/win32/shared/Windows_custom.h>

#define D3D_DISABLE_9EX
#include <d3d9.h>

#include <d3dx9.h>

#if BASALT_DEBUG_BUILD

#include <DxErr.h>

#endif

#if BASALT_DEBUG_BUILD

#define D3D9CHECK(x)                                                           \
  do {                                                                         \
    if (const HRESULT macroHr {x}; FAILED(macroHr)) {                          \
      DXTraceW(__FILE__, __LINE__, macroHr, nullptr, TRUE);                    \
    }                                                                          \
  } while (false)

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
