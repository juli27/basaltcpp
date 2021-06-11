#pragma once

#if BASALT_DEV_BUILD

namespace basalt::detail {

[[noreturn]] void fail_assert(const char* message, const char* file, int line,
                              const char* func);

} // namespace basalt::detail

#ifdef _MSC_VER
#define BASALT_FUNCTION_SIGNATURE __FUNCSIG__
#else
#define BASALT_FUNCTION_SIGNATURE __func__
#endif

#define BASALT_GLUE(a, b) a b

#define BASALT_ASSERT_IMPL1(expr)                                              \
  do {                                                                         \
    if (!(expr)) {                                                             \
      ::basalt::detail::fail_assert(#expr, __FILE__, __LINE__,                 \
                                    BASALT_FUNCTION_SIGNATURE);                \
    }                                                                          \
  } while (false)

#define BASALT_ASSERT_IMPL2(expr, msg)                                         \
  do {                                                                         \
    if (!(expr)) {                                                             \
      ::basalt::detail::fail_assert(msg " (" #expr ")", __FILE__, __LINE__,    \
                                    BASALT_FUNCTION_SIGNATURE);                \
    }                                                                          \
  } while (false)

#define BASALT_ASSERT_GET_IMPL(_1, _2, name, ...) name

// BASALT_GLUE to work around legacy msvc preprocessor bugs
#define BASALT_ASSERT(...)                                                     \
  BASALT_GLUE(BASALT_ASSERT_GET_IMPL(__VA_ARGS__, BASALT_ASSERT_IMPL2,         \
                                     BASALT_ASSERT_IMPL1),                     \
              (__VA_ARGS__))

#else // !BASALT_DEV_BUILD

#define BASALT_ASSERT(...)

#endif // BASALT_DEV_BUILD
