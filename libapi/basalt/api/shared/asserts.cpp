#include <basalt/api/shared/asserts.h>

#include <basalt/api/shared/log.h>

namespace basalt::detail {

#if BASALT_DEV_BUILD
void report_assert_failed(const char* message, const char* file, int line,
                          const char* func) {
  // TODO: allow to use asserts without logging initialized
  BASALT_LOG_FATAL("ASSERT FAILED: {}", message);
  BASALT_LOG_FATAL("\tfile: {}", file);
  BASALT_LOG_FATAL("\tline: {}", line);
  BASALT_LOG_FATAL("\tfunction: {}", func);
}
#endif // BASALT_DEV_BUILD

void report_crash(const char* message, const char* file, int line,
  const char* func) {
  // TODO: allow crashing without logging initialized
  // TODO: remove logging because it's unsafe during OOM situations
  // TODO: add crash handler/reporter
  BASALT_LOG_FATAL("HIT CRASH: {}", message);
  BASALT_LOG_FATAL("\tfile: {}", file);
  BASALT_LOG_FATAL("\tline: {}", line);
  BASALT_LOG_FATAL("\tfunction: {}", func);
}

} // namespace basalt::detail
