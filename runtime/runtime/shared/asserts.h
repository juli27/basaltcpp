#pragma once

#if BASALT_DEV_BUILD

namespace basalt::detail {

void fail_assert(
  const char* message, const char* file, int line, const char* func
);

} // namespace basalt::detail

#ifdef _MSC_VER
#define BASALT_FUNCTION_SIGNATURE __FUNCSIG__
#else
#define BASALT_FUNCTION_SIGNATURE __func__
#endif

#define BASALT_ASSERT(b)                                    \
  do {                                                      \
    if (!(b)) {                                             \
      ::basalt::detail::fail_assert(                        \
        #b, __FILE__, __LINE__, BASALT_FUNCTION_SIGNATURE); \
    }                                                       \
  } while (false)

#define BASALT_ASSERT_MSG(b, msg)                                        \
  do {                                                                   \
    if (!(b)) {                                                          \
      ::basalt::detail::fail_assert(                                     \
        #b " (" msg ")", __FILE__, __LINE__, BASALT_FUNCTION_SIGNATURE); \
    }                                                                    \
  } while (false)

#else // !BASALT_DEV_BUILD

#define BASALT_ASSERT(b)
#define BASALT_ASSERT_MSG(b, msg)

#endif // BASALT_DEV_BUILD
