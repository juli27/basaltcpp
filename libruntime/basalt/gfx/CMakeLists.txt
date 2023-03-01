add_subdirectory("backend")

target_sources(${CURRENT_TARGET} PRIVATE
  "debug.cpp"
  "debug.h"
  "device_state_cache.cpp"
  "device_state_cache.h"
  "filtering_command_list.cpp"
  "filtering_command_list.h"
  "utils.cpp"
  "utils.h"
)
