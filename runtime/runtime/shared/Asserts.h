#pragma once
#ifndef BASALT_SHARED_ASSERTS_H
#define BASALT_SHARED_ASSERTS_H

namespace basalt {

void fail_assert(
  const char* condition, const char* message, const char* file, int line,
  const char* func
);

} // namespace basalt

#ifdef _MSC_VER
#define BASALT_FUNCTION_SIGNATURE __FUNCSIG__
#else
#define BASALT_FUNCTION_SIGNATURE __func__
#endif

#define BASALT_ASSERT(b, msg) \
if (!!(b)) { \
} else { \
  ::basalt::fail_assert(#b, msg, __FILE__, __LINE__, BASALT_FUNCTION_SIGNATURE); \
}

#define BASALT_ASSERT_ARG_NOT_NULL(arg) \
  BASALT_ASSERT(arg, #arg " can't be null\n(Function: " __FUNCTION__ ")")

#endif  // !BASALT_SHARED_ASSERTS_H
