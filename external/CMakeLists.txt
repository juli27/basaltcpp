include(FetchContent)

add_subdirectory("DearImGui")

FetchContent_Declare(EnTT
  GIT_REPOSITORY https://github.com/skypjack/entt.git
  GIT_TAG        7a949dd32849e8230b15a5ff8ad1104c7f748c2a # v3.6.0
)
add_subdirectory("EnTT")

add_subdirectory("Fmt")
add_subdirectory("spdlog")

add_library(ExternalHeaderOnly INTERFACE)
target_include_directories(ExternalHeaderOnly INTERFACE "include")
