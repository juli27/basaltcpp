#pragma once
#ifndef BS_COMMON_ASSERTS_H
#define BS_COMMON_ASSERTS_H

#include <basalt/Log.h>

#include <cstdlib>

inline void fail_assert(
  const char* condition, const char* message, const char* file, const int line, const char* func
) {
  BS_FATAL("ASSERT FAILED: {} ({}) at {}:{} {}", condition, message, file, line, func);
  std::abort();
}

#if 1

#ifdef _MSC_VER
#define BS_FUNCTION_SIGNATURE __FUNCSIG__
#else
#define BS_FUNCTION_SIGNATURE __func__
#endif

#define BS_ASSERT(b, msg) \
if (b) { \
} else { \
  fail_assert(#b, msg, __FILE__, __LINE__, BS_FUNCTION_SIGNATURE); \
}

#else

#define BS_ASSERT(b, msg)

#endif

#define BS_ASSERT_ARG_NOT_NULL(arg) \
  BS_ASSERT(arg, #arg " can't be null\n(Function: " __FUNCTION__ ")")

#endif  // !BS_COMMON_ASSERTS_H
