FetchContent_GetProperties(fmt)
if (NOT fmt_POPULATED)
  FetchContent_Populate(fmt)

  add_library(fmt STATIC)
  add_library(fmt::fmt ALIAS fmt)

  target_link_libraries(fmt PRIVATE CommonFlags)

  target_include_directories(fmt PUBLIC "${fmt_SOURCE_DIR}/include")

  target_compile_options(fmt
    PRIVATE "/permissive-")

  set_property(TARGET fmt PROPERTY FOLDER "external")

  target_sources(fmt PRIVATE
    "${fmt_SOURCE_DIR}/include/fmt/args.h"
    "${fmt_SOURCE_DIR}/include/fmt/base.h"
    "${fmt_SOURCE_DIR}/include/fmt/chrono.h"
    "${fmt_SOURCE_DIR}/include/fmt/color.h"
    "${fmt_SOURCE_DIR}/include/fmt/compile.h"
    "${fmt_SOURCE_DIR}/include/fmt/core.h"
    "${fmt_SOURCE_DIR}/include/fmt/format.h"
    "${fmt_SOURCE_DIR}/include/fmt/format-inl.h"
    "${fmt_SOURCE_DIR}/include/fmt/os.h"
    "${fmt_SOURCE_DIR}/include/fmt/ostream.h"
    "${fmt_SOURCE_DIR}/include/fmt/printf.h"
    "${fmt_SOURCE_DIR}/include/fmt/ranges.h"
    "${fmt_SOURCE_DIR}/include/fmt/std.h"
    "${fmt_SOURCE_DIR}/include/fmt/xchar.h"
    "${fmt_SOURCE_DIR}/src/format.cc"
    "${fmt_SOURCE_DIR}/src/os.cc"
  )

  get_property(TARGET_SOURCES TARGET fmt PROPERTY SOURCES)
  source_group(TREE "${fmt_SOURCE_DIR}" FILES ${TARGET_SOURCES})
endif()
