#pragma once

#include "shared/Windows_custom.h"

#include <string>

namespace basalt::win32 {

std::string message_to_string(UINT message, WPARAM wParam, LPARAM lParam);

} // namespace basalt::win32
