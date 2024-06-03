# Generated by CMake

if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 2.8)
   message(FATAL_ERROR "CMake >= 2.8.0 required")
endif()
if(CMAKE_VERSION VERSION_LESS "2.8.3")
   message(FATAL_ERROR "CMake >= 2.8.3 required")
endif()
cmake_policy(PUSH)
cmake_policy(VERSION 2.8.3...3.23)
#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Protect against multiple inclusion, which would fail when already imported targets are added once more.
set(_cmake_targets_defined "")
set(_cmake_targets_not_defined "")
set(_cmake_expected_targets "")
foreach(_cmake_expected_target IN ITEMS next::next_plugin next::next_controlbridge next::next_databridge next::system_ECalMonitor next::system_SignalExporter next::system_RawFrameView next::system_RecExporter next::system_ReferenceCamera next::system_SignalList next::system_SignalProvider next::system_SignalRawData next::view3D_ArrayTpObjects next::view3D_CanTpObjects next::view3D_EgoVehicle next::view3D_Road next::view3D_PointCloud next::view3D_StructTpObjects next::system_PerformanceViewer)
  list(APPEND _cmake_expected_targets "${_cmake_expected_target}")
  if(TARGET "${_cmake_expected_target}")
    list(APPEND _cmake_targets_defined "${_cmake_expected_target}")
  else()
    list(APPEND _cmake_targets_not_defined "${_cmake_expected_target}")
  endif()
endforeach()
unset(_cmake_expected_target)
if(_cmake_targets_defined STREQUAL _cmake_expected_targets)
  unset(_cmake_targets_defined)
  unset(_cmake_targets_not_defined)
  unset(_cmake_expected_targets)
  unset(CMAKE_IMPORT_FILE_VERSION)
  cmake_policy(POP)
  return()
endif()
if(NOT _cmake_targets_defined STREQUAL "")
  string(REPLACE ";" ", " _cmake_targets_defined_text "${_cmake_targets_defined}")
  string(REPLACE ";" ", " _cmake_targets_not_defined_text "${_cmake_targets_not_defined}")
  message(FATAL_ERROR "Some (but not all) targets in this export set were already defined.\nTargets Defined: ${_cmake_targets_defined_text}\nTargets not yet defined: ${_cmake_targets_not_defined_text}\n")
endif()
unset(_cmake_targets_defined)
unset(_cmake_targets_not_defined)
unset(_cmake_expected_targets)


# Compute the installation prefix relative to this file.
get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)
if(_IMPORT_PREFIX STREQUAL "/")
  set(_IMPORT_PREFIX "")
endif()

# Create imported target next::next_plugin
add_library(next::next_plugin SHARED IMPORTED)

set_target_properties(next::next_plugin PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${_IMPORT_PREFIX}/include;${_IMPORT_PREFIX}/include"
  INTERFACE_LINK_LIBRARIES "mtscpp::mtscpp;next::next_sdk;next::next_udex"
)

# Create imported target next::next_controlbridge
add_executable(next::next_controlbridge IMPORTED)

set_target_properties(next::next_controlbridge PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${_IMPORT_PREFIX}/include"
)

# Create imported target next::next_databridge
add_executable(next::next_databridge IMPORTED)

set_target_properties(next::next_databridge PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${_IMPORT_PREFIX}/include"
)

# Create imported target next::system_ECalMonitor
add_library(next::system_ECalMonitor SHARED IMPORTED)

set_target_properties(next::system_ECalMonitor PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "BRIDGESDKSHARED"
)

# Create imported target next::system_SignalExporter
add_library(next::system_SignalExporter SHARED IMPORTED)

set_target_properties(next::system_SignalExporter PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "UNICODE;BRIDGESDKSHARED"
)

# Create imported target next::system_RawFrameView
add_library(next::system_RawFrameView SHARED IMPORTED)

set_target_properties(next::system_RawFrameView PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "BRIDGESDKSHARED"
)

# Create imported target next::system_RecExporter
add_library(next::system_RecExporter SHARED IMPORTED)

set_target_properties(next::system_RecExporter PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "UNICODE;BRIDGESDKSHARED"
)

# Create imported target next::system_ReferenceCamera
add_library(next::system_ReferenceCamera SHARED IMPORTED)

set_target_properties(next::system_ReferenceCamera PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "BRIDGESDKSHARED"
)

# Create imported target next::system_SignalList
add_library(next::system_SignalList SHARED IMPORTED)

set_target_properties(next::system_SignalList PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "BRIDGESDKSHARED"
)

# Create imported target next::system_SignalProvider
add_library(next::system_SignalProvider SHARED IMPORTED)

set_target_properties(next::system_SignalProvider PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "BRIDGESDKSHARED"
)

# Create imported target next::system_SignalRawData
add_library(next::system_SignalRawData SHARED IMPORTED)

set_target_properties(next::system_SignalRawData PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "BRIDGESDKSHARED"
)

# Create imported target next::view3D_ArrayTpObjects
add_library(next::view3D_ArrayTpObjects SHARED IMPORTED)

set_target_properties(next::view3D_ArrayTpObjects PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "BRIDGESDKSHARED"
)

# Create imported target next::view3D_CanTpObjects
add_library(next::view3D_CanTpObjects SHARED IMPORTED)

set_target_properties(next::view3D_CanTpObjects PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "BRIDGESDKSHARED"
)

# Create imported target next::view3D_EgoVehicle
add_library(next::view3D_EgoVehicle SHARED IMPORTED)

set_target_properties(next::view3D_EgoVehicle PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "BRIDGESDKSHARED"
)

# Create imported target next::view3D_Road
add_library(next::view3D_Road SHARED IMPORTED)

set_target_properties(next::view3D_Road PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "BRIDGESDKSHARED"
)

# Create imported target next::view3D_PointCloud
add_library(next::view3D_PointCloud SHARED IMPORTED)

set_target_properties(next::view3D_PointCloud PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "BRIDGESDKSHARED"
)

# Create imported target next::view3D_StructTpObjects
add_library(next::view3D_StructTpObjects SHARED IMPORTED)

set_target_properties(next::view3D_StructTpObjects PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "BRIDGESDKSHARED"
)

# Create imported target next::system_PerformanceViewer
add_library(next::system_PerformanceViewer SHARED IMPORTED)

set_target_properties(next::system_PerformanceViewer PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "BRIDGESDKSHARED"
)

# Load information for each installed configuration.
file(GLOB _cmake_config_files "${CMAKE_CURRENT_LIST_DIR}/exported-targets-*.cmake")
foreach(_cmake_config_file IN LISTS _cmake_config_files)
  include("${_cmake_config_file}")
endforeach()
unset(_cmake_config_file)
unset(_cmake_config_files)

# Cleanup temporary variables.
set(_IMPORT_PREFIX)

# Loop over all imported files and verify that they actually exist
foreach(_cmake_target IN LISTS _cmake_import_check_targets)
  foreach(_cmake_file IN LISTS "_cmake_import_check_files_for_${_cmake_target}")
    if(NOT EXISTS "${_cmake_file}")
      message(FATAL_ERROR "The imported target \"${_cmake_target}\" references the file
   \"${_cmake_file}\"
but this file does not exist.  Possible reasons include:
* The file was deleted, renamed, or moved to another location.
* An install or uninstall procedure did not complete successfully.
* The installation package was faulty and contained
   \"${CMAKE_CURRENT_LIST_FILE}\"
but not all the files it references.
")
    endif()
  endforeach()
  unset(_cmake_file)
  unset("_cmake_import_check_files_for_${_cmake_target}")
endforeach()
unset(_cmake_target)
unset(_cmake_import_check_targets)

# Make sure the targets which have been exported in some other
# export set exist.
unset(${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE_targets)
foreach(_target "next::next_sdk" "next::next_udex" )
  if(NOT TARGET "${_target}" )
    set(${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE_targets "${${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE_targets} ${_target}")
  endif()
endforeach()

if(DEFINED ${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE_targets)
  if(CMAKE_FIND_PACKAGE_NAME)
    set( ${CMAKE_FIND_PACKAGE_NAME}_FOUND FALSE)
    set( ${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE "The following imported targets are referenced, but are missing: ${${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE_targets}")
  else()
    message(FATAL_ERROR "The following imported targets are referenced, but are missing: ${${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE_targets}")
  endif()
endif()
unset(${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE_targets)

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
cmake_policy(POP)