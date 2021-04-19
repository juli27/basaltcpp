add_subdirectory("base")
add_subdirectory("win32utilities")
add_subdirectory("gfx")
add_subdirectory("win32")

target_sources(${CURRENT_TARGET} PRIVATE
  "engine.cpp"
)
