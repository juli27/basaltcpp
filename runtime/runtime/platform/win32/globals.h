#pragma once
#ifndef BASALT_WIN32_PLATFORM_H
#define BASALT_WIN32_PLATFORM_H

#include "runtime/shared/win32/Windows_custom.h"

namespace basalt::platform {

extern HINSTANCE sInstance;
extern int sShowCommand;

} // namespace basalt::platform

#endif // !BASALT_WIN32_PLATFORM_H
