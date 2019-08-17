#pragma once
#ifndef BS_COMMON_ASSERTS_H
#define BS_COMMON_ASSERTS_H

#include <cstdlib>

#include <basalt/Log.h>


#define BS_RELEASE_ASSERT(b, msg) \
  if (b) {                        \
  } else {                        \
    BS_FATAL(msg);                \
    std::abort();                 \
  }


#if defined(BS_DEBUG_BUILD)
#define BS_ASSERTS_ENABLED 1
#endif // defined(BS_DEBUG_BUILD)


#if BS_ASSERTS_ENABLED

#define BS_ASSERT(b, msg) BS_RELEASE_ASSERT(b, msg)

#else

#define BS_ASSERT(b, msg)

#endif


#define BS_ASSERT_ARG_NOT_NULL(arg) \
  BS_ASSERT(arg, #arg " can't be null\n(Function: " __FUNCTION__ ")")

#endif  // !BS_COMMON_ASSERTS_H
