#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "next::next_sdk" for configuration "Release"
set_property(TARGET next::next_sdk APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(next::next_sdk PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/next_sdk.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/next_sdk.dll"
  )

list(APPEND _cmake_import_check_targets next::next_sdk )
list(APPEND _cmake_import_check_files_for_next::next_sdk "${_IMPORT_PREFIX}/lib/next_sdk.lib" "${_IMPORT_PREFIX}/bin/next_sdk.dll" )

# Import target "next::next_appsupport" for configuration "Release"
set_property(TARGET next::next_appsupport APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(next::next_appsupport PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/next_appsupport.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/next_appsupport.dll"
  )

list(APPEND _cmake_import_check_targets next::next_appsupport )
list(APPEND _cmake_import_check_files_for_next::next_appsupport "${_IMPORT_PREFIX}/lib/next_appsupport.lib" "${_IMPORT_PREFIX}/bin/next_appsupport.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
