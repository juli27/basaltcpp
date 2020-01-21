target_sources(Runtime
  PRIVATE
    "Asserts.cpp"
    "Color.cpp"
    "Log.cpp"
  PUBLIC
    "Asserts.h"
    "Color.h"
    "Config.h"
    "Exceptions.h"
    "Handle.h"
    "HandlePool.h"
    "Log.h"
    "Types.h")

add_subdirectory("win32")
