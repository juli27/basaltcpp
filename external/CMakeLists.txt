add_library(ExternalHeaderOnly INTERFACE)

add_subdirectory("DearImGui")
add_subdirectory("Fmt")

target_include_directories(ExternalHeaderOnly INTERFACE "include")
