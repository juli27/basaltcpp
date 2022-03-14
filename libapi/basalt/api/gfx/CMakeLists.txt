target_sources(${CURRENT_TARGET} PRIVATE
  "camera.cpp"
  "camera.h"
  "device_state_cache.cpp"
  "device_state_cache.h"
  "drawable.h"
  "filtering_command_list.cpp"
  "filtering_command_list.h"
  "resource_cache.cpp"
  "resource_cache.h"
  "solid_color_view.cpp"
  "solid_color_view.h"
  "surface.cpp"
  "surface.h"
  "types.h"
)

add_subdirectory("backend")
