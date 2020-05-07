#include "Asserts.h"

#include "Log.h"

#include <cstdlib>

namespace basalt::detail {

void fail_assert(
  const char* message, const char* file, const int line, const char* func
) {
  BASALT_LOG_FATAL("ASSERT FAILED: {} at {}:{} {}", message, file, line, func);

  std::abort();
}

} // namespace basalt::detail
