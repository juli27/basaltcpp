FetchContent_GetProperties(spdlog)
if (NOT spdlog_POPULATED)
  FetchContent_Populate(spdlog)

  add_library(spdlog STATIC)
  add_library(spdlog::spdlog ALIAS spdlog)
  set(CURRENT_TARGET spdlog)

  target_link_libraries(${CURRENT_TARGET}
    PUBLIC
      fmt::fmt
    PRIVATE
      Cpp17Conformance
      CommonPrivate
      ControlFlowGuard
  )

  target_include_directories(${CURRENT_TARGET} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include" # override tweakme.h
    "${spdlog_SOURCE_DIR}/include"
  )

  target_compile_definitions(${CURRENT_TARGET} PUBLIC "SPDLOG_COMPILED_LIB")

  set_target_properties(${CURRENT_TARGET} PROPERTIES
    FOLDER "external"
    STATIC_LIBRARY_OPTIONS "/WX"
  )

  target_sources(${CURRENT_TARGET} PRIVATE
    "${spdlog_SOURCE_DIR}/src/async.cpp"
    "${spdlog_SOURCE_DIR}/src/cfg.cpp"
    "${spdlog_SOURCE_DIR}/src/color_sinks.cpp"
    "${spdlog_SOURCE_DIR}/src/file_sinks.cpp"
    "${spdlog_SOURCE_DIR}/src/spdlog.cpp"
    "${spdlog_SOURCE_DIR}/src/stdout_sinks.cpp"
  )

  get_target_property(TARGET_SOURCES ${CURRENT_TARGET} SOURCES)
  source_group(TREE "${spdlog_SOURCE_DIR}" FILES ${TARGET_SOURCES})

  target_sources(${CURRENT_TARGET} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include/spdlog/tweakme.h"
  )
endif()
