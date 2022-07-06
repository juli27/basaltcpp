add_subdirectory("backend")

target_sources(${CURRENT_TARGET} PRIVATE
  "resource_cache.cpp"
)
