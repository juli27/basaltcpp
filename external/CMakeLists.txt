add_subdirectory("DearImGui")
add_subdirectory("Fmt")
add_subdirectory("spdlog")

add_library(ExternalHeaderOnly INTERFACE)

target_include_directories(ExternalHeaderOnly INTERFACE "include")
