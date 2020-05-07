#pragma once
#ifndef BASALT_RUNTIME_PLATFORM_WIN32_GLOBALS_H
#define BASALT_RUNTIME_PLATFORM_WIN32_GLOBALS_H

#include "runtime/shared/win32/Windows_custom.h"

namespace basalt::platform {

extern HINSTANCE sInstance;
extern int sShowCommand;

} // namespace basalt::platform

#endif // !BASALT_RUNTIME_PLATFORM_WIN32_GLOBALS_H
