#pragma once

#include <basalt/win32/shared/Windows_custom.h>

#include <string>

namespace basalt {

auto message_to_string(UINT message, WPARAM wParam, LPARAM lParam)
  -> std::string;

} // namespace basalt
