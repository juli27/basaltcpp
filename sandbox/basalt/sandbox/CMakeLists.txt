add_subdirectory("d3d9")
add_subdirectory("samples")
add_subdirectory("tribase")

target_sources(${CURRENT_TARGET} PRIVATE
  "sandbox.cpp"
  "sandbox.h"
)
