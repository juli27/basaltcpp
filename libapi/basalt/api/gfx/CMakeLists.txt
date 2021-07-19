target_sources(${CURRENT_TARGET} PRIVATE
  "camera.cpp"
  "camera.h"
  "command_list_recorder.cpp"
  "command_list_recorder.h"
  "device_state_cache.cpp"
  "device_state_cache.h"
  "drawable.h"
  "resource_cache.cpp"
  "resource_cache.h"
  "scene_view.cpp"
  "scene_view.h"
  "solid_color_view.cpp"
  "solid_color_view.h"
  "surface.cpp"
  "surface.h"
  "types.h"
)

add_subdirectory("backend")
