#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "next::next" for configuration "Release"
set_property(TARGET next::next APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(next::next PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/next.lib"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "next::next_udex"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/next.dll"
  )

list(APPEND _cmake_import_check_targets next::next )
list(APPEND _cmake_import_check_files_for_next::next "${_IMPORT_PREFIX}/lib/next.lib" "${_IMPORT_PREFIX}/bin/next.dll" )

# Import target "next::next_container" for configuration "Release"
set_property(TARGET next::next_container APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(next::next_container PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/next_container.exe"
  )

list(APPEND _cmake_import_check_targets next::next_container )
list(APPEND _cmake_import_check_files_for_next::next_container "${_IMPORT_PREFIX}/bin/next_container.exe" )

# Import target "next::matchbox_example_app" for configuration "Release"
set_property(TARGET next::matchbox_example_app APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(next::matchbox_example_app PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/examples/matchbox_example_app/matchbox_example_app.exe"
  )

list(APPEND _cmake_import_check_targets next::matchbox_example_app )
list(APPEND _cmake_import_check_files_for_next::matchbox_example_app "${_IMPORT_PREFIX}/examples/matchbox_example_app/matchbox_example_app.exe" )

# Import target "next::rec_generator" for configuration "Release"
set_property(TARGET next::rec_generator APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(next::rec_generator PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/examples/rec_generator/rec_generator.exe"
  )

list(APPEND _cmake_import_check_targets next::rec_generator )
list(APPEND _cmake_import_check_files_for_next::rec_generator "${_IMPORT_PREFIX}/examples/rec_generator/rec_generator.exe" )

# Import target "next::next_example_node" for configuration "Release"
set_property(TARGET next::next_example_node APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(next::next_example_node PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/examples/next_example_node/next_example_node.exe"
  )

list(APPEND _cmake_import_check_targets next::next_example_node )
list(APPEND _cmake_import_check_files_for_next::next_example_node "${_IMPORT_PREFIX}/examples/next_example_node/next_example_node.exe" )

# Import target "next::test_component_node" for configuration "Release"
set_property(TARGET next::test_component_node APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(next::test_component_node PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/examples/test_component_node/test_component_node.exe"
  )

list(APPEND _cmake_import_check_targets next::test_component_node )
list(APPEND _cmake_import_check_files_for_next::test_component_node "${_IMPORT_PREFIX}/examples/test_component_node/test_component_node.exe" )

# Import target "next::lib_test_component_node" for configuration "Release"
set_property(TARGET next::lib_test_component_node APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(next::lib_test_component_node PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/examples/test_component_node/lib_test_component_node.lib"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "next::next;next::next_udex"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/examples/test_component_node/lib_test_component_node.dll"
  )

list(APPEND _cmake_import_check_targets next::lib_test_component_node )
list(APPEND _cmake_import_check_files_for_next::lib_test_component_node "${_IMPORT_PREFIX}/examples/test_component_node/lib_test_component_node.lib" "${_IMPORT_PREFIX}/examples/test_component_node/lib_test_component_node.dll" )

# Import target "next::next_subscriber_app" for configuration "Release"
set_property(TARGET next::next_subscriber_app APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(next::next_subscriber_app PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/examples/next_subscriber_app/next_subscriber_app.exe"
  )

list(APPEND _cmake_import_check_targets next::next_subscriber_app )
list(APPEND _cmake_import_check_files_for_next::next_subscriber_app "${_IMPORT_PREFIX}/examples/next_subscriber_app/next_subscriber_app.exe" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
