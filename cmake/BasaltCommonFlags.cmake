include_guard(GLOBAL)

add_library(CommonFlags INTERFACE)
target_compile_definitions(CommonFlags INTERFACE
  "_UNICODE"
  "UNICODE"
)

if(MSVC)
  target_compile_options(CommonFlags INTERFACE
    # input flags
    "/utf-8"

    # language flags
    #"/permissive-" # causes issues with old Windows SDKs
    "/Zc:__cplusplus"
    "/Zc:enumTypes" # could cause issues with the Windows SDK
    "/Zc:externC"
    "/Zc:externConstexpr"
    "/Zc:gotoScope"
    "/Zc:hiddenFriend"
    "/Zc:inline"
    "/Zc:lambda"
    "/Zc:nrvo"
    #"/Zc:preprocessor" # causes issues with old Windows SDK
    "/Zc:referenceBinding"
    "/Zc:rvalueCast"
    "/Zc:strictStrings"
    "/Zc:templateScope"
    "/Zc:ternary"
    "/Zc:throwingNew"

    # codegen flags
    "/fp:fast"
    "/GF"
    "/Gy"
    "$<$<CONFIG:RelWithDebInfo>:/Ob2>"

    # security flags
    "/sdl"
    "/guard:cf"

    # diagnostic flags
    "/FC" # Full path of source code file in diagnostics
    "/W4"
    "/w44062" # enumerator in a switch is not handled
    "/w14165" # 'HRESULT' is being converted to 'bool'
    "/w34191" # 'operator': unsafe conversion
    "/w44242" # conversion from 'type1' to 'type2', possible loss of data
  )

  target_link_options(CommonFlags INTERFACE
    # security flags
    "/GUARD:CF"
  )
endif()

if(BASALT_BUILD_WARNINGS_AS_ERRORS)
  if(MSVC)
    target_compile_options(CommonFlags INTERFACE
      "/WX"
      "/options:strict"
    )
    target_link_options(CommonFlags INTERFACE "/WX")
  endif()
endif()
