#pragma once
#ifndef BASALT_GFX_BACKEND_D3D9_HEADER_H
#define BASALT_GFX_BACKEND_D3D9_HEADER_H

#include <basalt/shared/Win32APIHeader.h>

#ifdef BS_DEBUG_BUILD
#define D3D_DEBUG_INFO
#endif

#define D3D_DISABLE_9EX
#include <d3d9.h>

#include <d3dx9.h>

#endif // !BASALT_GFX_BACKEND_D3D9_HEADER_H
