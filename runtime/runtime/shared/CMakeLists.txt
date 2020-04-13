target_sources(Runtime
  PRIVATE
    "Asserts.cpp"
    "Log.cpp"
  PUBLIC
    "Asserts.h"
    "Color.h"
    "Config.h"
    "Exceptions.h"
    "Handle.h"
    "HandlePool.h"
    "Log.h"
    "Size2D.h"
    "Types.h")

add_subdirectory("win32")
