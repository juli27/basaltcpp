add_library(LibWin32 STATIC)
add_library(Basalt::LibWin32 ALIAS LibWin32)

# Dependencies
find_package(DirectXSDK MODULE REQUIRED "d3dx9" "dxerr")
## TODO: move to find module?
set_package_properties(DirectXSDK PROPERTIES
  DESCRIPTION "Microsoft DirectX SDK"
  URL "https://www.microsoft.com/en-us/download/details.aspx?id=6812"
  PURPOSE "Dependency of the Direct3D 9 gfx backend")

target_link_libraries(LibWin32
  PUBLIC
    Basalt::LibAPI
    Basalt::LibRuntime
  PRIVATE
    CommonFlags
    "d3d9"
    DirectXSDK::d3dx9
    DirectXSDK::dxerr
    "legacy_stdio_definitions")

target_include_directories(LibWin32 PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}")

target_compile_features(LibWin32 PUBLIC cxx_std_17)
target_compile_definitions(LibWin32 PRIVATE
  "BASALT_BUILD"
  "$<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:BASALT_DEV_BUILD>"
  "$<$<CONFIG:Debug>:BASALT_DEBUG_BUILD>")

set_property(TARGET LibWin32 PROPERTY FOLDER "platformlibs")
set_property(TARGET LibWin32 PROPERTY VS_JUST_MY_CODE_DEBUGGING
              "$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>")

add_subdirectory("basalt")

get_property(TARGET_SOURCES TARGET LibWin32 PROPERTY SOURCES)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/basalt" FILES ${TARGET_SOURCES})
