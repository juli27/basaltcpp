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
    ${APP_NAME}
    Basalt::LibAPI
    Basalt::LibRuntime
    Basalt::LibWin32
  )

  target_compile_definitions(${CURRENT_TARGET} PRIVATE
    "$<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:BASALT_DEV_BUILD>"
    "$<$<CONFIG:Debug>:BASALT_DEBUG_BUILD>"
  )

  target_compile_features(${CURRENT_TARGET} PRIVATE cxx_std_17)
  target_compile_options(${CURRENT_TARGET}
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
  target_link_options(${CURRENT_TARGET} PRIVATE
    "/WX"
    "$<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:/DEBUG:FASTLINK>"
    "$<$<NOT:$<CONFIG:Debug>>:/OPT:REF;/OPT:ICF>"
    "$<$<CONFIG:Release>:/DEBUG:FULL>"
  )

  set_target_properties(${CURRENT_TARGET} PROPERTIES
    FOLDER ${APP_NAME}
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    VS_JUST_MY_CODE_DEBUGGING "$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>"
  )

  target_sources(${CURRENT_TARGET} PRIVATE
    "${CMAKE_SOURCE_DIR}/launchers/win32/App.manifest"
    "${CMAKE_SOURCE_DIR}/launchers/win32/main.cpp"
  )

  get_target_property(TARGET_SOURCES ${CURRENT_TARGET} SOURCES)
  source_group(TREE "${CMAKE_SOURCE_DIR}/launchers/win32" FILES ${TARGET_SOURCES})
endfunction()
