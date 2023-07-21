add_subdirectory("backend")

target_sources(${CURRENT_TARGET} PRIVATE
  "environment.cpp"
  "gfx_system.cpp"
  "resource_cache.cpp"
)
