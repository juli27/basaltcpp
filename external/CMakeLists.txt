include(FetchContent)

FetchContent_Declare(DearImGui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        45acd5e0e82f4c954432533ae9985ff0e1aad6d5 # v1.92.2b
)
FetchContent_Declare(EnTT
  GIT_REPOSITORY https://github.com/skypjack/entt.git
  GIT_TAG        d4014c74dc3793aba95ae354d6e23a026c2796db # v3.15.0
)
FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        40626af88bd7df9a5fb80be7b25ac85b122d6c21 # v11.2.0
)
FetchContent_Declare(GSL
  GIT_REPOSITORY https://github.com/microsoft/GSL.git
  GIT_TAG        7e0943d20d3082b4f350a7e0c3088d2388e934de
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
