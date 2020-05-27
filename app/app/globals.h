#pragma once
#ifndef BASALT_APP_GLOBALS_H
#define BASALT_APP_GLOBALS_H

#include "runtime/platform/Platform.h"

#include "runtime/shared/Config.h"

#include <vector>

namespace basalt {

namespace gfx {

struct View;

} // namespace gfx

namespace win32 {

extern WindowMode sWindowMode;

extern std::vector<platform::PlatformEventCallback> sEventListener;

extern gfx::View sCurrentView;

} // namespace win32
} // namespace basalt

#endif // !BASALT_APP_GLOBALS_H
