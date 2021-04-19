#include <basalt/api/shared/asserts.h>

#if BASALT_DEV_BUILD

#include <basalt/api/shared/log.h>

#include <cstdlib>

namespace basalt::detail {

void fail_assert(const char* message, const char* file, const int line,
                 const char* func) {
  BASALT_LOG_FATAL("ASSERT FAILED: {}", message);
  BASALT_LOG_FATAL("\tfile: {}", file);
  BASALT_LOG_FATAL("\tline: {}", line);
  BASALT_LOG_FATAL("\tfunction: {}", func);

  std::abort();
}

} // namespace basalt::detail

#endif // BASALT_DEV_BUILD
