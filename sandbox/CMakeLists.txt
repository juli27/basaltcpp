include(BasaltApp)

basalt_add_app(Sandbox)
add_library(Basalt::Sandbox ALIAS Sandbox)

target_link_libraries(Sandbox PRIVATE
  CommonFlags
  tomlplusplus::tomlplusplus
)

target_include_directories(Sandbox
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_compile_definitions(Sandbox PRIVATE
  "$<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:BASALT_DEV_BUILD>"
  "$<$<CONFIG:Debug>:BASALT_DEBUG_BUILD>")

target_compile_features(Sandbox PRIVATE cxx_std_17)

if(MSVC)
  target_compile_options(Sandbox PRIVATE
    "/permissive-"
    "/Zc:preprocessor")
endif()

set_property(TARGET Sandbox PROPERTY VS_JUST_MY_CODE_DEBUGGING
              "$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>")

add_subdirectory("basalt/sandbox")

get_target_property(TARGET_SOURCES Sandbox SOURCES)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/basalt/sandbox"
  FILES ${TARGET_SOURCES})

target_sources(Sandbox PRIVATE "bootstrap.cpp")
source_group("" FILES "bootstrap.cpp")
