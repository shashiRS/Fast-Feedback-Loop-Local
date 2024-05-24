#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "next::next_plugin" for configuration "Release"
set_property(TARGET next::next_plugin APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(next::next_plugin PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/next_plugin.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/next_plugin.dll"
  )

list(APPEND _cmake_import_check_targets next::next_plugin )
list(APPEND _cmake_import_check_files_for_next::next_plugin "${_IMPORT_PREFIX}/lib/next_plugin.lib" "${_IMPORT_PREFIX}/bin/next_plugin.dll" )

# Import target "next::next_controlbridge" for configuration "Release"
set_property(TARGET next::next_controlbridge APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(next::next_controlbridge PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/next_controlbridge.exe"
  )

list(APPEND _cmake_import_check_targets next::next_controlbridge )
list(APPEND _cmake_import_check_files_for_next::next_controlbridge "${_IMPORT_PREFIX}/bin/next_controlbridge.exe" )

# Import target "next::next_databridge" for configuration "Release"
set_property(TARGET next::next_databridge APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(next::next_databridge PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/next_databridge.exe"
  )

list(APPEND _cmake_import_check_targets next::next_databridge )
list(APPEND _cmake_import_check_files_for_next::next_databridge "${_IMPORT_PREFIX}/bin/next_databridge.exe" )

# Import target "next::system_ECalMonitor" for configuration "Release"
set_property(TARGET next::system_ECalMonitor APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(next::system_ECalMonitor PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "eCAL::core;next::next_plugin"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/plugins/core/system_ECalMonitor/system_ECalMonitor.dll"
  )

list(APPEND _cmake_import_check_targets next::system_ECalMonitor )
list(APPEND _cmake_import_check_files_for_next::system_ECalMonitor "${_IMPORT_PREFIX}/bin/plugins/core/system_ECalMonitor/system_ECalMonitor.dll" )

# Import target "next::system_SignalExporter" for configuration "Release"
set_property(TARGET next::system_SignalExporter APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(next::system_SignalExporter PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "next::next_plugin;next::next_appsupport"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/plugins/core/system_SignalExporter/system_SignalExporter.dll"
  )

list(APPEND _cmake_import_check_targets next::system_SignalExporter )
list(APPEND _cmake_import_check_files_for_next::system_SignalExporter "${_IMPORT_PREFIX}/bin/plugins/core/system_SignalExporter/system_SignalExporter.dll" )

# Import target "next::system_RecExporter" for configuration "Release"
set_property(TARGET next::system_RecExporter APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(next::system_RecExporter PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "next::next_plugin;next::next_appsupport;mts_swc::mts_swc"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/plugins/core/system_RecExporter/system_RecExporter.dll"
  )

list(APPEND _cmake_import_check_targets next::system_RecExporter )
list(APPEND _cmake_import_check_files_for_next::system_RecExporter "${_IMPORT_PREFIX}/bin/plugins/core/system_RecExporter/system_RecExporter.dll" )

# Import target "next::system_ReferenceCamera" for configuration "Release"
set_property(TARGET next::system_ReferenceCamera APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(next::system_ReferenceCamera PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "next::next_plugin"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/plugins/core/system_ReferenceCamera/system_ReferenceCamera.dll"
  )

list(APPEND _cmake_import_check_targets next::system_ReferenceCamera )
list(APPEND _cmake_import_check_files_for_next::system_ReferenceCamera "${_IMPORT_PREFIX}/bin/plugins/core/system_ReferenceCamera/system_ReferenceCamera.dll" )

# Import target "next::system_SignalList" for configuration "Release"
set_property(TARGET next::system_SignalList APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(next::system_SignalList PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "next::next_plugin"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/plugins/core/system_SignalList/system_SignalList.dll"
  )

list(APPEND _cmake_import_check_targets next::system_SignalList )
list(APPEND _cmake_import_check_files_for_next::system_SignalList "${_IMPORT_PREFIX}/bin/plugins/core/system_SignalList/system_SignalList.dll" )

# Import target "next::system_SignalProvider" for configuration "Release"
set_property(TARGET next::system_SignalProvider APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(next::system_SignalProvider PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "next::next_udex;next::next_plugin"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/plugins/core/system_SignalProvider/system_SignalProvider.dll"
  )

list(APPEND _cmake_import_check_targets next::system_SignalProvider )
list(APPEND _cmake_import_check_files_for_next::system_SignalProvider "${_IMPORT_PREFIX}/bin/plugins/core/system_SignalProvider/system_SignalProvider.dll" )

# Import target "next::system_SignalRawData" for configuration "Release"
set_property(TARGET next::system_SignalRawData APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(next::system_SignalRawData PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "next::next_plugin"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/plugins/core/system_SignalRawData/system_SignalRawData.dll"
  )

list(APPEND _cmake_import_check_targets next::system_SignalRawData )
list(APPEND _cmake_import_check_files_for_next::system_SignalRawData "${_IMPORT_PREFIX}/bin/plugins/core/system_SignalRawData/system_SignalRawData.dll" )

# Import target "next::view3D_ArrayTpObjects" for configuration "Release"
set_property(TARGET next::view3D_ArrayTpObjects APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(next::view3D_ArrayTpObjects PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "next::next_plugin"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/plugins/core/view3D_ArrayTpObjects/view3D_ArrayTpObjects.dll"
  )

list(APPEND _cmake_import_check_targets next::view3D_ArrayTpObjects )
list(APPEND _cmake_import_check_files_for_next::view3D_ArrayTpObjects "${_IMPORT_PREFIX}/bin/plugins/core/view3D_ArrayTpObjects/view3D_ArrayTpObjects.dll" )

# Import target "next::view3D_CanTpObjects" for configuration "Release"
set_property(TARGET next::view3D_CanTpObjects APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(next::view3D_CanTpObjects PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "next::next_plugin;next::next_appsupport"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/plugins/core/view3D_CanTpObjects/view3D_CanTpObjects.dll"
  )

list(APPEND _cmake_import_check_targets next::view3D_CanTpObjects )
list(APPEND _cmake_import_check_files_for_next::view3D_CanTpObjects "${_IMPORT_PREFIX}/bin/plugins/core/view3D_CanTpObjects/view3D_CanTpObjects.dll" )

# Import target "next::view3D_EgoVehicle" for configuration "Release"
set_property(TARGET next::view3D_EgoVehicle APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(next::view3D_EgoVehicle PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "next::next_plugin;next::next_appsupport"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/plugins/core/view3D_EgoVehicle/view3D_EgoVehicle.dll"
  )

list(APPEND _cmake_import_check_targets next::view3D_EgoVehicle )
list(APPEND _cmake_import_check_files_for_next::view3D_EgoVehicle "${_IMPORT_PREFIX}/bin/plugins/core/view3D_EgoVehicle/view3D_EgoVehicle.dll" )

# Import target "next::view3D_Road" for configuration "Release"
set_property(TARGET next::view3D_Road APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(next::view3D_Road PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "next::next_plugin;next::next_appsupport;next::next_udex"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/plugins/core/view3D_Road/view3D_Road.dll"
  )

list(APPEND _cmake_import_check_targets next::view3D_Road )
list(APPEND _cmake_import_check_files_for_next::view3D_Road "${_IMPORT_PREFIX}/bin/plugins/core/view3D_Road/view3D_Road.dll" )

# Import target "next::view3D_PointCloud" for configuration "Release"
set_property(TARGET next::view3D_PointCloud APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(next::view3D_PointCloud PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "next::next_plugin;next::next_appsupport;next::next_udex"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/plugins/core/view3D_PointCloud/view3D_PointCloud.dll"
  )

list(APPEND _cmake_import_check_targets next::view3D_PointCloud )
list(APPEND _cmake_import_check_files_for_next::view3D_PointCloud "${_IMPORT_PREFIX}/bin/plugins/core/view3D_PointCloud/view3D_PointCloud.dll" )

# Import target "next::view3D_StructTpObjects" for configuration "Release"
set_property(TARGET next::view3D_StructTpObjects APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(next::view3D_StructTpObjects PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "next::next_plugin;next::next_appsupport;next::next_udex"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/plugins/core/view3D_StructTpObjects/view3D_StructTpObjects.dll"
  )

list(APPEND _cmake_import_check_targets next::view3D_StructTpObjects )
list(APPEND _cmake_import_check_files_for_next::view3D_StructTpObjects "${_IMPORT_PREFIX}/bin/plugins/core/view3D_StructTpObjects/view3D_StructTpObjects.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
