include(FetchContent)

FetchContent_Declare(DearImGui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        993fa347495860ed44b83574254ef2a317d0c14f # v1.91.6
)
FetchContent_Declare(EnTT
  GIT_REPOSITORY https://github.com/skypjack/entt.git
  GIT_TAG        4a2d1a8541228a90e02a873dba6a980506c42c03 # v3.14.0
)
FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        8303d140a1a11f19b982a9f664bbe59a1ccda3f4 # v11.1.2
)
FetchContent_Declare(GSL
  GIT_REPOSITORY https://github.com/microsoft/GSL.git
  GIT_TAG        355982daf6c54ccb11bef8a1c511be2622dec402
)
FetchContent_Declare(spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        8e5613379f5140fefb0b60412fbf1f5406e7c7f8 # v1.15.0
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
