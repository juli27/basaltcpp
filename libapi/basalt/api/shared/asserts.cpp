#include <basalt/api/shared/asserts.h>

#include <basalt/api/shared/log.h>

#include <basalt/api/base/platform.h>

#include <cstdlib>

namespace basalt::detail {

#if BASALT_DEV_BUILD
auto report_assert_failed(char const* message, char const* file, int line,
                          char const* func) -> void {
  // TODO: allow to use asserts without logging initialized
  BASALT_LOG_FATAL("ASSERT FAILED: {}", message);
  BASALT_LOG_FATAL("\tfile: {}", file);
  BASALT_LOG_FATAL("\tline: {}", line);
  BASALT_LOG_FATAL("\tfunction: {}", func);
}
#endif // BASALT_DEV_BUILD

auto report_crash(char const* message, char const* file, int line,
                  char const* func) -> void {
  // TODO: allow crashing without logging initialized
  // TODO: remove logging because it's unsafe during OOM situations
  // TODO: add crash handler/reporter
  BASALT_LOG_FATAL("HIT CRASH: {}", message);
  BASALT_LOG_FATAL("\tfile: {}", file);
  BASALT_LOG_FATAL("\tline: {}", line);
  BASALT_LOG_FATAL("\tfunction: {}", func);
}

auto should_break_debugger() -> bool {
  return Platform::is_debugger_attached();
}

auto crash() -> void {
  std::abort();
}

} // namespace basalt::detail
