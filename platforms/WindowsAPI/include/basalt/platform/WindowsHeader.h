#pragma once
#ifndef BS_PLATFORM_WINDOWSHEADER_H
#define BS_PLATFORM_WINDOWSHEADER_H

// 0x0600 == _WIN32_WINNT_VISTA
#define WINVER 0x0600
#define _WIN32_WINNT 0x0600

#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
//#define NOVIRTUALKEYCODES
//#define NOWINMESSAGES
//#define NOWINSTYLES
//#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NOSYSCOMMANDS
#define NORASTEROPS
#define NOSHOWWINDOW
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
//#define NOGDI // because of d3d9 header in gfx context
#define NOKERNEL
//#define NOUSER
//#define NONLS
//#define NOMB
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
#define NOOPENFILE
//#define NOMSG
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
//#define NOTEXTMETRIC // because of d3d9 header in gfx context
#define NOWH
#define NOWINOFFSETS
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX

#include <Windows.h>

// interferes with KeyCode::DELETE
#if defined(DELETE)
#undef DELETE
#endif

// interferes with PlatformEventType::MOUSE_MOVED
#if defined(MOUSE_MOVED)
#undef MOUSE_MOVED
#endif

// for CommandLineToArgvW
#include <shellapi.h>

// for GET_X_LPARAM and GET_Y_LPARAM
#include <windowsx.h>

#endif // !BS_PLATFORM_WINDOWSHEADER_H
