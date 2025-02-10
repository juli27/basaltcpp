add_library(LanguageFlags INTERFACE)
if(MSVC)
  target_compile_options(LanguageFlags INTERFACE
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
    "/Zc:throwingNew")
endif()

add_library(CodegenFlags INTERFACE)
if(MSVC)
  target_compile_options(CodegenFlags INTERFACE
    "/fp:fast"
    "/GF"
    "/Gy"
    "$<$<CONFIG:RelWithDebInfo>:/Ob2>")
endif()

add_library(SecurityFlags INTERFACE)
if(MSVC)
  target_compile_options(SecurityFlags INTERFACE
    "/sdl"
    "/guard:cf")
  target_link_options(SecurityFlags INTERFACE "/GUARD:CF")
endif()

add_library(DiagnosticFlags INTERFACE)
if(MSVC)
  target_compile_options(DiagnosticFlags INTERFACE
    "/FC"
    "/W4"
    "/w44062" # enumerator in a switch is not handled
    "/w14165" # 'HRESULT' is being converted to 'bool'
    "/w34191" # 'operator': unsafe conversion
    "/w44242") # conversion from 'type1' to 'type2', possible loss of data
endif()

if(BASALT_BUILD_WARNINGS_AS_ERRORS)
  if(MSVC)
    target_compile_options(DiagnosticFlags INTERFACE
      "/WX"
      "/options:strict")
    target_link_options(DiagnosticFlags INTERFACE "/WX")
  endif()
endif()

add_library(CommonFlags INTERFACE)
target_link_libraries(CommonFlags INTERFACE
  LanguageFlags
  CodegenFlags
  SecurityFlags
  DiagnosticFlags)

target_compile_definitions(CommonFlags INTERFACE
  "_UNICODE"
  "UNICODE")

if(MSVC)
  target_compile_options(CommonFlags INTERFACE "/utf-8")
endif()
