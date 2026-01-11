set(SPDLOG_DISABLE_DEFAULT_LOGGER ON)
set(SPDLOG_FMT_EXTERNAL ON)
set(SPDLOG_NO_ATOMIC_LEVELS ON)
set(SPDLOG_NO_THREAD_ID ON)
set(SPDLOG_WCHAR_FILENAMES ON)
set(SPDLOG_WCHAR_SUPPORT ON)

FetchContent_MakeAvailable(spdlog)

target_link_libraries(spdlog PRIVATE CommonFlags)

target_compile_definitions(spdlog PUBLIC "SPDLOG_NO_SOURCE_LOC")

if(MSVC)
  target_compile_options(spdlog PRIVATE "/permissive-")
endif()

set_property(TARGET spdlog PROPERTY FOLDER "external")
