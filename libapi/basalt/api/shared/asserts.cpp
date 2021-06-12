#include <basalt/api/shared/asserts.h>

#include <basalt/api/shared/log.h>

#include <basalt/api/base/platform.h>

#include <cstdlib>

#if BASALT_DEV_BUILD
#define BASALT_BREAK_DEBUGGER() __debugbreak()
#endif

namespace basalt::detail {

namespace {

[[noreturn]] void do_crash() {
#if BASALT_DEV_BUILD
  if (Platform::is_debugger_attached()) {
    BASALT_BREAK_DEBUGGER();
  }
#endif

  std::abort();
}

} // namespace

#if BASALT_DEV_BUILD
void fail_assert(const char* message, const char* file, const int line,
                 const char* func) {
  // TODO: allow to use asserts without logging initialized
  BASALT_LOG_FATAL("ASSERT FAILED: {}", message);
  BASALT_LOG_FATAL("\tfile: {}", file);
  BASALT_LOG_FATAL("\tline: {}", line);
  BASALT_LOG_FATAL("\tfunction: {}", func);

  do_crash();
}
#endif // BASALT_DEV_BUILD

void crash(const char* message, const char* file, int line, const char* func) {
  // TODO: allow crashing without logging initialized
  // TODO: remove logging because it's unsafe during OOM situations
  // TODO: add crash handler/reporter
  BASALT_LOG_FATAL("HIT CRASH: {}", message);
  BASALT_LOG_FATAL("\tfile: {}", file);
  BASALT_LOG_FATAL("\tline: {}", line);
  BASALT_LOG_FATAL("\tfunction: {}", func);

  do_crash();
}

} // namespace basalt::detail
