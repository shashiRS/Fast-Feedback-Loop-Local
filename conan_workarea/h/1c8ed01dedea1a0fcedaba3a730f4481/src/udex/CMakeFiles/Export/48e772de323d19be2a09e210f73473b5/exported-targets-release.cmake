#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "next::next_udex" for configuration "Release"
set_property(TARGET next::next_udex APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(next::next_udex PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/next_udex.lib"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "next::next_appsupport;eCAL::core"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/next_udex.dll"
  )

list(APPEND _cmake_import_check_targets next::next_udex )
list(APPEND _cmake_import_check_files_for_next::next_udex "${_IMPORT_PREFIX}/lib/next_udex.lib" "${_IMPORT_PREFIX}/bin/next_udex.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
