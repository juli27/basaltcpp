add_subdirectory("backend")

target_sources(${CURRENT_TARGET} PRIVATE
  "gfx_system.cpp"
  "resource_cache.cpp"
)
