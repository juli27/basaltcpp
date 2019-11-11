#pragma once
#ifndef BASALT_COMMON_ASSERTS_H
#define BASALT_COMMON_ASSERTS_H

#include <basalt/Log.h>

#include <cstdlib>

inline void fail_assert(
  const char* condition, const char* message, const char* file, const int line, const char* func
) {
  BASALT_LOG_FATAL("ASSERT FAILED: {} ({}) at {}:{} {}", condition, message,
    file, line, func);
  std::abort();
}

#if 1

#ifdef _MSC_VER
#define BASALT_FUNCTION_SIGNATURE __FUNCSIG__
#else
#define BASALT_FUNCTION_SIGNATURE __func__
#endif

#define BASALT_ASSERT(b, msg) \
if (b) { \
} else { \
  fail_assert(#b, msg, __FILE__, __LINE__, BASALT_FUNCTION_SIGNATURE); \
}

#else

#define BASALT_ASSERT(b, msg)

#endif

#define BASALT_ASSERT_ARG_NOT_NULL(arg) \
  BASALT_ASSERT(arg, #arg " can't be null\n(Function: " __FUNCTION__ ")")

#endif  // !BASALT_COMMON_ASSERTS_H
