#pragma once
#ifndef BASALT_WIN32_UTIL_H
#define BASALT_WIN32_UTIL_H

#include <basalt/shared/Win32APIHeader.h>

#include <string>

auto create_utf8_from_wide(std::wstring_view src) noexcept -> std::string;
auto create_winapi_error_message(DWORD errorCode) noexcept -> std::string;

#endif // BASALT_WIN32_UTIL_H
