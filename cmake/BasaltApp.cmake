function(basalt_add_app APP_NAME)
  add_library(${APP_NAME} STATIC)

  target_link_libraries(${APP_NAME} PUBLIC Basalt::LibAPI)

  set_property(TARGET ${APP_NAME} PROPERTY FOLDER ${APP_NAME})

  set(EXE_NAME "${APP_NAME}.Win32")
  add_executable(${EXE_NAME} WIN32)

  target_link_libraries(${EXE_NAME} PRIVATE
    ControlFlowGuard
    CommonPrivate
    ${APP_NAME}
    Basalt::LibAPI
    Basalt::LibRuntime
    Basalt::LibWin32
  )

  target_compile_definitions(${EXE_NAME} PRIVATE
    "$<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:BASALT_DEV_BUILD>"
    "$<$<CONFIG:Debug>:BASALT_DEBUG_BUILD>"
  )

  target_compile_features(${EXE_NAME} PRIVATE cxx_std_17)
  target_compile_options(${EXE_NAME}
    PRIVATE
      "/w44062;/w14165;/w34191;/w44242"
      "/Zc:__cplusplus"
      "/Zc:externC"
      "/Zc:externConstexpr"
      "/Zc:hiddenFriend"
      "/Zc:inline"
      "/Zc:lambda"
      "/Zc:nrvo"
      "/Zc:preprocessor-"
      "/Zc:referenceBinding"
      "/Zc:rvalueCast"
      "/Zc:strictStrings"
      "/Zc:ternary"
      "/Zc:throwingNew"
  )

  # TODO: /LTCG:incremental with ninja
  target_link_options(${EXE_NAME} PRIVATE
    "/WX"
    "$<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:/DEBUG:FASTLINK>"
    "$<$<NOT:$<CONFIG:Debug>>:/OPT:REF;/OPT:ICF>"
    "$<$<CONFIG:Release>:/DEBUG:FULL>"
  )

  set_property(TARGET ${EXE_NAME} PROPERTY FOLDER ${APP_NAME})
  set_property(TARGET ${EXE_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY
               "${CMAKE_SOURCE_DIR}")
  set_property(TARGET ${EXE_NAME} PROPERTY VS_JUST_MY_CODE_DEBUGGING
              "$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>")

  set(BASALT_WIN32_MANIFEST_APP_NAME "basalt.${APP_NAME}")
  configure_file("${PROJECT_SOURCE_DIR}/platformlibs/libwin32/app.manifest.in"
    "app.manifest"
    @ONLY)

  configure_file("${PROJECT_SOURCE_DIR}/platformlibs/libwin32/main.cpp"
    "main.cpp"
    COPYONLY)

  target_sources(${EXE_NAME}
    PRIVATE
      "${CMAKE_CURRENT_BINARY_DIR}/app.manifest"
      "${CMAKE_CURRENT_BINARY_DIR}/main.cpp")
  source_group(""
    FILES
      "${CMAKE_CURRENT_BINARY_DIR}/app.manifest"
      "${CMAKE_CURRENT_BINARY_DIR}/main.cpp")
endfunction()
