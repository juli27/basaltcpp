target_sources(${CURRENT_TARGET} PRIVATE
  "camera.cpp"
  "camera.h"
  "device_state_cache.cpp"
  "device_state_cache.h"
  "filtering_command_list.cpp"
  "filtering_command_list.h"
  "resource_cache.h"
  "types.h"
)

add_subdirectory("backend")
