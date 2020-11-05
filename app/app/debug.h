#pragma once

#include "shared/Windows_custom.h"

#include <string>

namespace basalt {

auto message_to_string(UINT message, WPARAM wParam, LPARAM lParam)
  -> std::string;

} // namespace basalt
