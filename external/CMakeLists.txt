include(FetchContent)

FetchContent_Declare(DearImGui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        ad5d1a8429ea219d3d34e6a36a48918650402697 # v1.83
)
FetchContent_Declare(EnTT
  GIT_REPOSITORY https://github.com/skypjack/entt.git
  GIT_TAG        5d15a3d69f433a0e3fce266caaeb87c77c10453c # v3.7.1
)

add_subdirectory("DearImGui")
add_subdirectory("EnTT")
add_subdirectory("Fmt")
add_subdirectory("spdlog")

add_library(ExternalHeaderOnly INTERFACE)
target_include_directories(ExternalHeaderOnly INTERFACE "include")
