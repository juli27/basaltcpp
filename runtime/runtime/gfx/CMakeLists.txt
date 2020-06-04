target_sources(Runtime
  PRIVATE
    "Camera.cpp"
    "Gfx.cpp"
    "scene_view.cpp"
  PUBLIC
    "Camera.h"
    "Gfx.h"
    "scene_view.h"
    "types.h"
)

add_subdirectory("backend")
