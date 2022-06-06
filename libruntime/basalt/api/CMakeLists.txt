add_subdirectory("gfx/backend")

target_sources(${CURRENT_TARGET} PRIVATE
  "debug_ui.cpp"
  "engine.cpp"
)
