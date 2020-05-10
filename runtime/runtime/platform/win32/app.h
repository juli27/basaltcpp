#pragma once
#ifndef BASALT_RUNTIME_PLATFORM_WIN32_APP_H
#define BASALT_RUNTIME_PLATFORM_WIN32_APP_H

#include "runtime/shared/win32/Windows_custom.h"

namespace basalt::win32 {

void run(HINSTANCE instance, int showCommand);

} // namespace basalt::win32

#endif // BASALT_RUNTIME_PLATFORM_WIN32_APP_H
