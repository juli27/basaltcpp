#pragma once
#ifndef BASALT_WIN32_PLATFORM_H
#define BASALT_WIN32_PLATFORM_H

#include <basalt/shared/Win32APIHeader.h>

namespace basalt::platform {

extern HINSTANCE sInstance;
extern int sShowCommand;

} // namespace basalt::platform

#endif // !BASALT_WIN32_PLATFORM_H
