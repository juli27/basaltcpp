#pragma once

namespace basalt::detail {

#if BASALT_DEV_BUILD
auto report_assert_failed(char const* message, char const* file, int line,
                          char const* func) -> void;
#endif

auto report_crash(char const* message, char const* file, int line,
                  char const* func) -> void;

auto should_break_debugger() -> bool;

[[noreturn]]
auto crash() -> void;

} // namespace basalt::detail

#ifdef _MSC_VER
#define BASALT_FUNCTION_SIGNATURE __FUNCSIG__
#else
#define BASALT_FUNCTION_SIGNATURE __func__
#endif

#if BASALT_DEV_BUILD

#define BASALT_BREAK_DEBUGGER() __debugbreak()

// TODO: should we remove the debugger check and just crash while breaking
// instead of aborting ?
#define BASALT_DO_CRASH()                                                      \
  do {                                                                         \
    if (::basalt::detail::should_break_debugger()) {                           \
      BASALT_BREAK_DEBUGGER();                                                 \
    }                                                                          \
                                                                               \
    ::basalt::detail::crash();                                                 \
  } while (false)

#define BASALT_GLUE(a, b) a b

#define BASALT_ASSERT_IMPL1(expr)                                              \
  do {                                                                         \
    if (!(expr)) {                                                             \
      ::basalt::detail::report_assert_failed(#expr, __FILE__, __LINE__,        \
                                             BASALT_FUNCTION_SIGNATURE);       \
      BASALT_DO_CRASH();                                                       \
    }                                                                          \
  } while (false)

#define BASALT_ASSERT_IMPL2(expr, msg)                                         \
  do {                                                                         \
    if (!(expr)) {                                                             \
      ::basalt::detail::report_assert_failed(                                  \
        msg " (" #expr ")", __FILE__, __LINE__, BASALT_FUNCTION_SIGNATURE);    \
      BASALT_DO_CRASH();                                                       \
    }                                                                          \
  } while (false)

#define BASALT_ASSERT_GET_IMPL(_1, _2, name, ...) name

// BASALT_GLUE to work around legacy msvc preprocessor bugs
#define BASALT_ASSERT(...)                                                     \
  BASALT_GLUE(BASALT_ASSERT_GET_IMPL(__VA_ARGS__, BASALT_ASSERT_IMPL2,         \
                                     BASALT_ASSERT_IMPL1),                     \
              (__VA_ARGS__))

#define BASALT_ASSERT_IF(cond, ...)                                            \
  do {                                                                         \
    if (cond) {                                                                \
      BASALT_ASSERT(__VA_ARGS__);                                              \
    }                                                                          \
  } while (false)

#else // !BASALT_DEV_BUILD

#define BASALT_DO_CRASH()                                                      \
  do {                                                                         \
    std::abort();                                                              \
  } while (false)

#define BASALT_ASSERT(...)
#define BASALT_ASSERT_IF(...)

#endif // BASALT_DEV_BUILD

#define BASALT_CRASH(msg)                                                      \
  do {                                                                         \
    ::basalt::detail::report_crash(msg, __FILE__, __LINE__,                    \
                                   BASALT_FUNCTION_SIGNATURE);                 \
    BASALT_DO_CRASH();                                                         \
  } while (false)
