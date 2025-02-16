add_library(LibRuntime STATIC)
add_library(Basalt::LibRuntime ALIAS LibRuntime)

target_link_libraries(LibRuntime
  PUBLIC Basalt::LibAPI
  PRIVATE CommonFlags
)

target_include_directories(LibRuntime PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_compile_definitions(LibRuntime PRIVATE "BASALT_BUILD")

target_compile_features(LibRuntime PUBLIC cxx_std_17)

if(MSVC)
  target_compile_options(LibRuntime PRIVATE
    "/permissive-"
    "/Zc:preprocessor"
  )
endif()

set_property(TARGET LibRuntime PROPERTY
  VS_JUST_MY_CODE_DEBUGGING "$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>"
)

add_subdirectory("basalt")

get_property(TARGET_SOURCES TARGET LibRuntime PROPERTY SOURCES)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/basalt" FILES ${TARGET_SOURCES})
