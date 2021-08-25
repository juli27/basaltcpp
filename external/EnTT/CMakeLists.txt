FetchContent_GetProperties(EnTT)
if (NOT entt_POPULATED)
  FetchContent_Populate(EnTT)

  file(COPY "config.h" DESTINATION "${entt_SOURCE_DIR}/src/entt/config")

  add_subdirectory(${entt_SOURCE_DIR} ${entt_BINARY_DIR})
endif()
