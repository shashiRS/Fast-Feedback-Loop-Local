#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "next::next_player" for configuration "Release"
set_property(TARGET next::next_player APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(next::next_player PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/next_player.exe"
  )

list(APPEND _cmake_import_check_targets next::next_player )
list(APPEND _cmake_import_check_files_for_next::next_player "${_IMPORT_PREFIX}/bin/next_player.exe" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
