include(FetchContent)

add_subdirectory("DearImGui")

FetchContent_Declare(EnTT
  GIT_REPOSITORY https://github.com/skypjack/entt.git
  GIT_TAG        5d15a3d69f433a0e3fce266caaeb87c77c10453c # v3.7.1
)
add_subdirectory("EnTT")

add_subdirectory("Fmt")
add_subdirectory("spdlog")

add_library(ExternalHeaderOnly INTERFACE)
target_include_directories(ExternalHeaderOnly INTERFACE "include")
