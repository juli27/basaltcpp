FetchContent_GetProperties(fmt)
if (NOT fmt_POPULATED)
  FetchContent_Populate(fmt)

  add_library(fmt STATIC)
  add_library(fmt::fmt ALIAS fmt)
  set(CURRENT_TARGET fmt)

  target_link_libraries(${CURRENT_TARGET}
    PRIVATE
      CommonPrivate
      Cpp17_Strict
      ControlFlowGuard
      WarningsAsErrors
  )

  target_include_directories(${CURRENT_TARGET}
    PUBLIC "${fmt_SOURCE_DIR}/include"
  )

  set_target_properties(${CURRENT_TARGET} PROPERTIES
    FOLDER "external"
    STATIC_LIBRARY_OPTIONS "/WX"
  )

  target_sources(${CURRENT_TARGET} PRIVATE
    "${fmt_SOURCE_DIR}/include/fmt/chrono.h"
    "${fmt_SOURCE_DIR}/include/fmt/color.h"
    "${fmt_SOURCE_DIR}/include/fmt/compile.h"
    "${fmt_SOURCE_DIR}/include/fmt/core.h"
    "${fmt_SOURCE_DIR}/include/fmt/format.h"
    "${fmt_SOURCE_DIR}/include/fmt/format-inl.h"
    "${fmt_SOURCE_DIR}/include/fmt/locale.h"
    "${fmt_SOURCE_DIR}/include/fmt/os.h"
    "${fmt_SOURCE_DIR}/include/fmt/ostream.h"
    "${fmt_SOURCE_DIR}/include/fmt/posix.h"
    "${fmt_SOURCE_DIR}/include/fmt/printf.h"
    "${fmt_SOURCE_DIR}/include/fmt/ranges.h"
    "${fmt_SOURCE_DIR}/src/format.cc"
    "${fmt_SOURCE_DIR}/src/os.cc"
  )

  get_target_property(TARGET_SOURCES ${CURRENT_TARGET} SOURCES)
  source_group(TREE "${fmt_SOURCE_DIR}" FILES ${TARGET_SOURCES})
endif()
