add_subdirectory("samples")
add_subdirectory("tribase")

target_sources(Sandbox PRIVATE
  "d3d9_tutorials.cpp"
  "d3d9_tutorials.h"
  "sandbox.cpp"
  "sandbox.h"
)
