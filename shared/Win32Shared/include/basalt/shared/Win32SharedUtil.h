#pragma once
#ifndef BASALT_WIN32_SHARED_H
#define BASALT_WIN32_SHARED_H

#include <string>

namespace basalt {

auto create_wide_from_utf8(std::string_view src) noexcept -> std::wstring;

} // namespace basalt

#endif // BASALT_WIN32_SHARED_H
