add_subdirectory("gfx")
add_subdirectory("scene")

target_sources(${CURRENT_TARGET} PRIVATE
  "debug_ui.cpp"
  "engine.cpp"
  "scene_view.cpp"
  "view.cpp"
)
