#include "util.h"

#include "shared/utils.h"

#include <basalt/api/base/asserts.h>
#include <basalt/api/base/log.h>
#include <basalt/api/base/platform.h>

#include <string>

using namespace std::literals;
using std::string;

/**
 * \brief Creates an error description from a Windows API error code.
 *
 * \param errorCode Windows API error code.
 * \return description string of the error.
 */
auto basalt::create_win32_error_message(DWORD const errorCode) noexcept
  -> string {
  WCHAR* buffer = nullptr;
  auto const numChars = FormatMessageW(
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS,
    nullptr, errorCode, 0u, reinterpret_cast<WCHAR*>(&buffer), 0u, nullptr);

  if (numChars == 0u) {
    return u8"FormatMessageW failed"s;
  }

  // use numChars because the buffer is NOT null terminated
  auto message = create_utf8_from_wide({buffer, numChars});

  LocalFree(buffer);

  return message;
}

#if BASALT_DEV_BUILD

auto basalt::detail::verify_win32_bool(BOOL const result) -> BOOL {
  if (!result) {
    BASALT_LOG_ERROR("win32 error: {}",
                     create_win32_error_message(GetLastError()));

    if (Platform::is_debugger_attached()) {
      BASALT_BREAK_DEBUGGER();
    }
  }

  return result;
}

#endif
