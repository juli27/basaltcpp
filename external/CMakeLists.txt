include(FetchContent)

FetchContent_Declare(DearImGui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        c71a50deb5ddf1ea386b91e60fa2e4a26d080074 # v1.87
)
FetchContent_Declare(EnTT
  GIT_REPOSITORY https://github.com/skypjack/entt.git
  GIT_TAG        8e0747fd505beb6861939a141ec0962c376d36f2 # v3.9.0
)
FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        b6f4ceaed0a0a24ccf575fab6c56dd50ccf6f1a9 # 8.1.1
)
FetchContent_Declare(GSL
  GIT_REPOSITORY https://github.com/microsoft/GSL.git
  GIT_TAG        d9fc52e20e32fcc804502480a781120b210afd41
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
