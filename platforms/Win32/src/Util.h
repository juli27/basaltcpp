#pragma once
#ifndef UTIL_H
#define UTIL_H

#include <string>

#include "Win32APIHeader.h"

auto create_wide_from_utf8(std::string_view src) noexcept -> std::wstring;

/**
 * \brief Converts a Windows API wide string to UTF-8.
 *
 * The converted string is returned in a new string object.
 * Fails with std::terminate when string storage allocation throws.
 *
 * \param src the wide string to convert.
 * \return the wide string converted to UTF-8.
 */
auto create_utf8_from_wide(std::wstring_view src) noexcept -> std::string;

/**
 * \brief Creates an error description from a Windows API error code.
 *
 * \param errorCode Windows API error code.
 * \return description string of the error.
 */
auto create_winapi_error_message(DWORD errorCode) noexcept -> std::string;

#endif // UTIL_H
