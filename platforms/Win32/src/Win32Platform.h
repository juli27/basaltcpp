#pragma once
#ifndef BASALT_WIN32_PLATFORM_H
#define BASALT_WIN32_PLATFORM_H

#include "Win32APIHeader.h"

#include <string>

namespace basalt::platform {

extern HINSTANCE sInstance;
extern int sShowCommand;
extern std::string sPlatformName;

} // namespace basalt::platform

#endif // !BASALT_WIN32_PLATFORM_H
