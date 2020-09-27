target_sources(Runtime
  PRIVATE
    "camera.cpp"
    "compositor.cpp"
    "draw_target.cpp"
    "scene_view.cpp"
  PUBLIC
    "camera.h"
    "compositor.h"
    "draw_target.h"
    "drawable.h"
    "scene_view.h"
    "types.h"
)

add_subdirectory("backend")
