target_sources(${CURRENT_TARGET} PRIVATE
  "camera.cpp"
  "camera.h"
  "gfx_system.h"
  "resource_cache.h"
  "types.h"
)

add_subdirectory("backend")
