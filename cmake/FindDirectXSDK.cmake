set(DirectXSDK_ROOT_DIR "$ENV{DXSDK_DIR}" CACHE FILEPATH
  "Path to the Microsoft DirectX SDK"
)

find_path(DirectXSDK_INCLUDE_DIR
  NAMES "d3dx9.h"
  PATHS "${DirectXSDK_ROOT_DIR}/Include"
  DOC "Microsoft DirectX SDK include directory"
)
mark_as_advanced(DirectXSDK_INCLUDE_DIR)

set(DirecXSDK_LIB_DIR "${DirectXSDK_ROOT_DIR}/Lib/x64")

foreach(FIND_COMPONENT ${DirectXSDK_FIND_COMPONENTS})
  if(${FIND_COMPONENT} STREQUAL "d3dx9")
    find_library(DirectXSDK_d3dx9_LIBRARY
      NAMES "d3dx9"
      PATHS "${DirecXSDK_LIB_DIR}"
    )
    mark_as_advanced(DirectXSDK_d3dx9_LIBRARY)

    find_library(DirectXSDK_d3dx9_LIBRARY_DEBUG
      NAMES "d3dx9d"
      PATHS "${DirecXSDK_LIB_DIR}"
    )
    mark_as_advanced(DirectXSDK_d3dx9_LIBRARY_DEBUG)
    set(DirectXSDK_${FIND_COMPONENT}_FOUND TRUE)
  elseif(${FIND_COMPONENT} STREQUAL "dxerr")
    find_library(DirectXSDK_dxerr_LIBRARY
      NAMES "dxerr"
      PATHS "${DirecXSDK_LIB_DIR}")
    mark_as_advanced(DirectXSDK_dxerr_LIBRARY)

    set(DirectXSDK_${FIND_COMPONENT}_FOUND TRUE)
  endif()
endforeach()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DirectXSDK
  REQUIRED_VARS
    DirectXSDK_ROOT_DIR
    DirectXSDK_INCLUDE_DIR
  HANDLE_COMPONENTS
)

if (DirectXSDK_FOUND)
  if (DirectXSDK_d3dx9_FOUND AND NOT TARGET DirectXSDK::d3dx9)
    add_library(DirectXSDK::d3dx9 UNKNOWN IMPORTED)
    target_include_directories(DirectXSDK::d3dx9 INTERFACE
      ${DirectXSDK_INCLUDE_DIR}
    )

    if (DirectXSDK_d3dx9_LIBRARY)
      set_target_properties(DirectXSDK::d3dx9 PROPERTIES
        IMPORTED_LOCATION ${DirectXSDK_d3dx9_LIBRARY}
      )
    endif()

    if (DirectXSDK_d3dx9_LIBRARY_DEBUG)
      set_property(TARGET DirectXSDK::d3dx9 APPEND PROPERTY
        IMPORTED_CONFIGURATIONS DEBUG
      )
      set_target_properties(DirectXSDK::d3dx9 PROPERTIES
        IMPORTED_LOCATION_DEBUG ${DirectXSDK_d3dx9_LIBRARY_DEBUG}
      )
    endif()
  endif()

  if (DirectXSDK_dxerr_FOUND AND NOT TARGET DirectXSDK::dxerr)
    add_library(DirectXSDK::dxerr UNKNOWN IMPORTED)
    target_include_directories(DirectXSDK::dxerr INTERFACE
      ${DirectXSDK_INCLUDE_DIR}
    )

    if (DirectXSDK_dxerr_LIBRARY)
      set_target_properties(DirectXSDK::dxerr PROPERTIES
        IMPORTED_LOCATION ${DirectXSDK_dxerr_LIBRARY}
      )
    endif()
  endif()
endif()
