#pragma once
#ifndef BASALT_RUNTIME_PLATFORM_WIN32_APP_H
#define BASALT_RUNTIME_PLATFORM_WIN32_APP_H

#include "runtime/shared/Config.h"

namespace basalt::win32 {

void create_main_window(const Config& config);

} // namespace basalt::win32

#endif // BASALT_RUNTIME_PLATFORM_WIN32_APP_H
