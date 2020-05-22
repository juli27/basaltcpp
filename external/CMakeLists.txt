add_library(ExternalHeaderOnly INTERFACE)

add_subdirectory("DearImGui")
add_subdirectory("Fmt")
add_subdirectory("spdlog")

target_include_directories(ExternalHeaderOnly INTERFACE "include")
