add_subdirectory("backend")

target_sources(${CURRENT_TARGET} PRIVATE
  "debug.cpp"
  "debug.h"
  "types.h"
  "utils.cpp"
  "utils.h"
)
