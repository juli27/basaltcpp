include(FetchContent)

FetchContent_Declare(DearImGui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        dbb5eeaadffb6a3ba6a60de1290312e5802dba5a # v1.91.8
)
FetchContent_Declare(EnTT
  GIT_REPOSITORY https://github.com/skypjack/entt.git
  GIT_TAG        4a2d1a8541228a90e02a873dba6a980506c42c03 # v3.14.0
)
FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        40626af88bd7df9a5fb80be7b25ac85b122d6c21 # v11.2.0
)
FetchContent_Declare(GSL
  GIT_REPOSITORY https://github.com/microsoft/GSL.git
  GIT_TAG        2828399820ef4928cc89b65605dca5dc68efca6e
)
FetchContent_Declare(spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        6fa36017cfd5731d617e1a934f0e5ea9c4445b13 # v1.15.3
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
