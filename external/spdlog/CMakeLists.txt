FetchContent_GetProperties(spdlog)
if (NOT spdlog_POPULATED)
  FetchContent_Populate(spdlog)

  add_library(spdlog STATIC)
  add_library(spdlog::spdlog ALIAS spdlog)

  target_link_libraries(spdlog
    PUBLIC
      fmt::fmt
    PRIVATE
      CommonFlags
  )

  target_include_directories(spdlog PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include" # override tweakme.h
    "${spdlog_SOURCE_DIR}/include"
  )

  target_compile_definitions(spdlog PUBLIC "SPDLOG_COMPILED_LIB")
  target_compile_options(spdlog PRIVATE "/permissive-")

  set_property(TARGET spdlog PROPERTY FOLDER "external")

  target_sources(spdlog PRIVATE
    "${spdlog_SOURCE_DIR}/src/async.cpp"
    "${spdlog_SOURCE_DIR}/src/cfg.cpp"
    "${spdlog_SOURCE_DIR}/src/color_sinks.cpp"
    "${spdlog_SOURCE_DIR}/src/file_sinks.cpp"
    "${spdlog_SOURCE_DIR}/src/spdlog.cpp"
    "${spdlog_SOURCE_DIR}/src/stdout_sinks.cpp"
  )

  get_property(TARGET_SOURCES TARGET spdlog PROPERTY SOURCES)
  source_group(TREE "${spdlog_SOURCE_DIR}" FILES ${TARGET_SOURCES})

  target_sources(spdlog PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include/spdlog/tweakme.h"
  )
endif()
