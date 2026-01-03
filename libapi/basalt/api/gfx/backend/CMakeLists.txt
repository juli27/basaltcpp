target_sources(LibAPI PRIVATE
  "adapter.h"
  "buffer.h"
  "command_list.h"
  "pipeline.h"
  "types.h"
  "vertex_layout.h"
)

add_subdirectory("ext")
