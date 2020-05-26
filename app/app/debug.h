#pragma once
#ifndef BASALT_APP_DEBUG_H
#define BASALT_APP_DEBUG_H

#include "shared/Windows_custom.h"

#include <string>

namespace basalt::win32 {

std::string message_to_string(UINT message, WPARAM wParam, LPARAM lParam);

} // namespace basalt::win32

#endif // BASALT_APP_DEBUG_H
