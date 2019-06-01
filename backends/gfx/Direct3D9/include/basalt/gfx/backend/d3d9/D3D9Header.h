#pragma once
#ifndef BS_GFX_D3D9API_H
#define BS_GFX_D3D9API_H

// 0x0600 == _WIN32_WINNT_VISTA
#define WINVER 0x0600
#define _WIN32_WINNT 0x0600

#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
#define NOMENUS
#define NOICONS
#define NOSYSCOMMANDS
#define NORASTEROPS
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOWH
#define NOWINOFFSETS

#include <Windows.h>

// interferes with KeyCode::DELETE
#if defined(DELETE)
#undef DELETE
#endif

// interferes with PlatformEventType::MOUSE_MOVED
#if defined(MOUSE_MOVED)
#undef MOUSE_MOVED
#endif

#ifdef BS_DEBUG_BUILD
#define D3D_DEBUG_INFO
#endif

#define D3D_DISABLE_9EX
#include <d3d9.h>

#include <d3dx9.h>

#endif // !BS_GFX_D3D9API_H
