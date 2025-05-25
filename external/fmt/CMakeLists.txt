set (FMT_INSTALL OFF)
FetchContent_MakeAvailable(fmt)

target_link_libraries(fmt PRIVATE CommonFlags)
target_compile_options(fmt PRIVATE "/permissive-")

set_property(TARGET fmt PROPERTY FOLDER "external")
