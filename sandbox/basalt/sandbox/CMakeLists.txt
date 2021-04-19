add_subdirectory("d3d9")

target_sources(${CURRENT_TARGET} PRIVATE
  "sandbox.cpp"
  "sandbox.h"
  "test_case.h"
)
