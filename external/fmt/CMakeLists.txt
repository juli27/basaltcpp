set (FMT_INSTALL OFF)
FetchContent_MakeAvailable(fmt)

target_link_libraries(fmt PRIVATE CommonFlags)

if(MSVC)
  target_compile_options(fmt PRIVATE
    "/permissive-"
    "/wd4834" # discarding return value of function with 'nodiscard' attribute
  )
endif()

set_property(TARGET fmt PROPERTY FOLDER "external")
