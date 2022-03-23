include(FetchContent)

FetchContent_Declare(DearImGui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        ad5d1a8429ea219d3d34e6a36a48918650402697 # v1.83
)
FetchContent_Declare(EnTT
  GIT_REPOSITORY https://github.com/skypjack/entt.git
  GIT_TAG        5d15a3d69f433a0e3fce266caaeb87c77c10453c # v3.7.1
)
FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        d141cdbeb0fb422a3fb7173b285fd38e0d1772dc # 8.0.1
)
FetchContent_Declare(GSL
  GIT_REPOSITORY https://github.com/microsoft/GSL.git
  GIT_TAG        c31a9ad5e8e99da8567237a7ba51a7d8f419e5bf
)
FetchContent_Declare(spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        eb3220622e73a4889eee355ffa37972b3cac3df5 # v1.9.2
)

add_subdirectory("DearImGui")
add_subdirectory("EnTT")
add_subdirectory("fmt")
FetchContent_MakeAvailable(GSL)
add_subdirectory("spdlog")
