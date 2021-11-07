include(FetchContent)

FetchContent_Declare(DearImGui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        ad5d1a8429ea219d3d34e6a36a48918650402697 # v1.83
)
FetchContent_Declare(EnTT
  GIT_REPOSITORY https://github.com/skypjack/entt.git
  GIT_TAG        5d15a3d69f433a0e3fce266caaeb87c77c10453c # v3.7.1
)
FetchContent_Declare(GSL
  GIT_REPOSITORY https://github.com/microsoft/GSL.git
  GIT_TAG        c31a9ad5e8e99da8567237a7ba51a7d8f419e5bf
)

add_subdirectory("DearImGui")
add_subdirectory("EnTT")
add_subdirectory("Fmt")
FetchContent_MakeAvailable(GSL)
add_subdirectory("spdlog")
