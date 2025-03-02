# TODO: set other options from tweakme header
set(SPDLOG_FMT_EXTERNAL ON)

FetchContent_MakeAvailable(spdlog)

target_link_libraries(spdlog PRIVATE CommonFlags)

target_include_directories(spdlog BEFORE PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include" # override tweakme.h
)

target_compile_options(spdlog PRIVATE "/permissive-")

set_property(TARGET spdlog PROPERTY FOLDER "external")

target_sources(spdlog PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include/spdlog/tweakme.h"
)
