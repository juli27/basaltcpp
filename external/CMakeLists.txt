include(FetchContent)

FetchContent_Declare(DearImGui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        6af38b1a431eafda4cd9a3ad5d2e6c35425c1a14 # v1.89.1 + 1
)
FetchContent_Declare(EnTT
  GIT_REPOSITORY https://github.com/skypjack/entt.git
  GIT_TAG        76298bc26ae7d47e5e75baeb9da1bd5bf17bdadc # v3.11
)
FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        a33701196adfad74917046096bf5a2aa0ab0bb50 # v9.1
)
FetchContent_Declare(GSL
  GIT_REPOSITORY https://github.com/microsoft/GSL.git
  GIT_TAG        517ed29228d18cf2c5004d10826090108e06f049
)
FetchContent_Declare(spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        ad0e89cbfb4d0c1ce4d097e134eb7be67baebb36 # v1.11
)

add_subdirectory("DearImGui")
FetchContent_MakeAvailable(EnTT)
add_subdirectory("fmt")
FetchContent_MakeAvailable(GSL)
add_subdirectory("spdlog")
