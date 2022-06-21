add_library(ControlFlowGuard INTERFACE)
target_compile_options(ControlFlowGuard INTERFACE "/guard:cf")
target_link_options(ControlFlowGuard INTERFACE "/GUARD:CF")

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

option(BASALT_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

if (BASALT_WARNINGS_AS_ERRORS)
  target_link_libraries(CommonPrivate INTERFACE WarningsAsErrors)
endif()
