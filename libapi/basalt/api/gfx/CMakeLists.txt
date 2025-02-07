add_subdirectory("backend")

target_sources(LibAPI PRIVATE
  "camera.cpp"
  "camera.h"
  "context.h"
  "environment.h"
  "gfx_system.h"
  "info.h"
  "resource_cache.h"
  "resources.h"
  "types.h"
)
