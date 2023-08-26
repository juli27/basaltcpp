add_subdirectory("backend")

target_sources(${CURRENT_TARGET} PRIVATE
  "context.cpp"
  "environment.cpp"
  "gfx_system.cpp"
  "resource_cache.cpp"
)
