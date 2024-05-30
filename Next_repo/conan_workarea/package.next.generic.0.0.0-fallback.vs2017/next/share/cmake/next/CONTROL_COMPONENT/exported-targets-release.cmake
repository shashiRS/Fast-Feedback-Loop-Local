#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "next::next_control" for configuration "Release"
set_property(TARGET next::next_control APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(next::next_control PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/next_control.lib"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "eCAL::core;next::next_appsupport;next::next_udex"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/next_control.dll"
  )

list(APPEND _cmake_import_check_targets next::next_control )
list(APPEND _cmake_import_check_files_for_next::next_control "${_IMPORT_PREFIX}/lib/next_control.lib" "${_IMPORT_PREFIX}/bin/next_control.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
