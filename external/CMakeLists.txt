include(FetchContent)

FetchContent_Declare(DearImGui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        8936b58fe26e8c3da834b8f60b06511d537b4c63 # v1.92.8
)
FetchContent_Declare(EnTT
  GIT_REPOSITORY https://github.com/skypjack/entt.git
  GIT_TAG        b4e58bdd364ad72246c123a0c28538eab3252672 # v3.16.0
)
FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        407c905e45ad75fc29bf0f9bb7c5c2fd3475976f # v12.1.0
)
FetchContent_Declare(GSL
  GIT_REPOSITORY https://github.com/microsoft/GSL.git
  GIT_TAG        b2f6bec48e27381b2e11d650ce7e4b899b446289
)
FetchContent_Declare(spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        79524ddd08a4ec981b7fea76afd08ee05f83755d # v1.17.0
)
FetchContent_Declare(tomlplusplus
  GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
  GIT_TAG        30172438cee64926dc41fdd9c11fb3ba5b2ba9de # v3.4.0
)

add_subdirectory("DearImGui")
FetchContent_MakeAvailable(EnTT)
add_subdirectory("fmt")
FetchContent_MakeAvailable(GSL)
add_subdirectory("spdlog")
FetchContent_MakeAvailable(tomlplusplus)
