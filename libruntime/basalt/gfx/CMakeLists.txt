add_subdirectory("backend")

target_sources(LibRuntime PRIVATE
  "context.cpp"
  "debug.cpp"
  "debug.h"
  "device_state_cache.cpp"
  "device_state_cache.h"
  "environment.cpp"
  "filtering_command_list.cpp"
  "filtering_command_list.h"
  "gfx_system.cpp"
  "material.cpp"
  "material_class.cpp"
  "resource_cache.cpp"
  "utils.cpp"
  "utils.h"
)
