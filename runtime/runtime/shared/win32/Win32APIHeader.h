#pragma once
#ifndef BASALT_WIN32_API_HEADER_H
#define BASALT_WIN32_API_HEADER_H

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
//#define NOSHOWWINDOW
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
//#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
//#define NOGDI
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
//#define NOTEXTMETRIC
#define NOWH
#define NOWINOFFSETS
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX

#include <Windows.h>

#endif // BASALT_WIN32_API_HEADER_H
