cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(Basalt VERSION 0.4 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include(FeatureSummary)

set(CMAKE_CXX_EXTENSIONS OFF)

include(CheckIPOSupported)
check_ipo_supported()
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Dependencies
find_package(DirectXSDK MODULE REQUIRED "d3dx9" "dxerr")
## TODO: move to find module?
set_package_properties(DirectXSDK PROPERTIES
  DESCRIPTION "Microsoft DirectX SDK"
  URL "https://www.microsoft.com/en-us/download/details.aspx?id=6812"
  PURPOSE "Dependency of the Direct3D 9 gfx backend"
)

# Needs to be linked into the final exe in order to enable it
add_library(ControlFlowGuard INTERFACE)
target_compile_options(ControlFlowGuard INTERFACE "/guard:cf")
target_link_options(ControlFlowGuard INTERFACE "/GUARD:CF")

# Strict C++17 conformance
add_library(Cpp17_Strict INTERFACE)
target_compile_features(Cpp17_Strict INTERFACE cxx_std_17)
target_compile_options(Cpp17_Strict INTERFACE "/permissive-" "/Zc:inline")

add_library(WarningsAsErrors INTERFACE)
target_compile_options(WarningsAsErrors INTERFACE "/WX")

add_library(CommonPrivate INTERFACE)
target_compile_definitions(CommonPrivate INTERFACE "_UNICODE" "UNICODE")
target_compile_options(CommonPrivate INTERFACE
  "/FC" "/fp:fast" "/Gy" "/sdl" "/W4"
  "$<$<CONFIG:Debug>:/GF>"
  "$<$<CONFIG:RelWithDebInfo>:/Ob2>"
  "$<$<CONFIG:Release>:/Zi>"
)

add_subdirectory("external" EXCLUDE_FROM_ALL)
add_subdirectory("libapi")
add_subdirectory("libruntime")
add_subdirectory("platformlibs")

set(BASALT_SOURCE_DIR ${CMAKE_SOURCE_DIR})

function(basalt_add_app APP_NAME)
  add_library(${APP_NAME} STATIC)
  set(CURRENT_TARGET ${APP_NAME} PARENT_SCOPE)

  target_link_libraries(${APP_NAME} PUBLIC Basalt::LibAPI)

  set_target_properties(${APP_NAME} PROPERTIES
    FOLDER ${APP_NAME}
  )

  set(EXE_NAME "${APP_NAME}.Win32")
  add_executable(${EXE_NAME} WIN32)
  set(CURRENT_TARGET ${EXE_NAME})

  target_link_libraries(${CURRENT_TARGET} PRIVATE
    ControlFlowGuard
    CommonPrivate
    WarningsAsErrors
    ${APP_NAME}
    Basalt::LibAPI
    Basalt::LibRuntime
    Basalt::LibWin32
  )

  target_compile_definitions(${CURRENT_TARGET} PRIVATE
    "$<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:BASALT_DEV_BUILD>"
    "$<$<CONFIG:Debug>:BASALT_DEBUG_BUILD>"
  )

  # TODO: /LTCG:incremental with ninja
  target_link_options(${CURRENT_TARGET} PRIVATE
    "/WX"
    "$<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:/DEBUG:FASTLINK>"
    "$<$<NOT:$<CONFIG:Debug>>:/OPT:REF;/OPT:ICF>"
    "$<$<CONFIG:Release>:/DEBUG:FULL>"
  )

  set_target_properties(${CURRENT_TARGET} PROPERTIES
    FOLDER ${APP_NAME}
    OUTPUT_NAME ${APP_NAME}
    RUNTIME_OUTPUT_DIRECTORY "win32"
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    VS_JUST_MY_CODE_DEBUGGING "$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>"
  )

  target_sources(${CURRENT_TARGET} PRIVATE
    "${BASALT_SOURCE_DIR}/launchers/win32/App.manifest"
    "${BASALT_SOURCE_DIR}/launchers/win32/main.cpp"
  )

  get_target_property(TARGET_SOURCES ${CURRENT_TARGET} SOURCES)
  source_group(TREE "${BASALT_SOURCE_DIR}/launchers/win32" FILES ${TARGET_SOURCES})
endfunction()

add_subdirectory("sandbox")

set_directory_properties(PROPERTIES VS_STARTUP_PROJECT Sandbox.Win32)

feature_summary(WHAT ALL)
