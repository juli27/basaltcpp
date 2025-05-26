add_subdirectory("shared")
add_subdirectory("samples")
add_subdirectory("tribase")

target_sources(Sandbox PRIVATE
  "sandbox.cpp"
  "sandbox.h"
  "settings.cpp"
  "settings.h"
  "settings_ui.cpp"
  "settings_ui.h"
)
