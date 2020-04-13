#include "Asserts.h"

#include "Log.h"

#include <cstdlib>

namespace basalt {

void fail_assert(
  const char* condition, const char* message, const char* file, const int line,
  const char* func
) {
  BASALT_LOG_FATAL(
    "ASSERT FAILED: {} ({}) at {}:{} {}", condition, message,
    file, line, func
  );

  std::abort();
}

} // namespace basalt
