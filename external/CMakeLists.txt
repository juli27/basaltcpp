include(FetchContent)

FetchContent_Declare(DearImGui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        6f7b5d0ee2fe9948ab871a530888a6dc5c960700 # v1.90.8
)
FetchContent_Declare(EnTT
  GIT_REPOSITORY https://github.com/skypjack/entt.git
  GIT_TAG        78213075654a688e9da6bc49f7f873d25c26d12c # v3.13.2
)
FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        e69e5f977d458f2650bb346dadf2ad30c5320281 # 10.2.1
)
FetchContent_Declare(GSL
  GIT_REPOSITORY https://github.com/microsoft/GSL.git
  GIT_TAG        b39e7e4b0987859f5b19ff7686b149c916588658
)
FetchContent_Declare(spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        27cb4c76708608465c413f6d0e6b8d99a4d84302 # v1.14.1
)

add_subdirectory("DearImGui")
FetchContent_MakeAvailable(EnTT)
add_subdirectory("fmt")
FetchContent_MakeAvailable(GSL)
add_subdirectory("spdlog")
