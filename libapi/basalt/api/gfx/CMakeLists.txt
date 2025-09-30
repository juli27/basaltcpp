add_subdirectory("backend")

target_sources(LibAPI PRIVATE
  "camera.cpp"
  "camera.h"
  "context.h"
  "environment.h"
  "gfx_system.h"
  "info.h"
  "material.h"
  "material_class.h"
  "mesh.h"
  "resource_cache.h"
  "types.h"
)
