# Install script for directory: D:/Next-python_API/next/src/bridges/plugins/core/view3D_PointCloud

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "D:/Next-python_API/next/conan_workarea/package.next.generic.0.0.0-fallback.vs2017")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "BRIDGES_COMPONENT" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/plugins/core/view3D_PointCloud" TYPE SHARED_LIBRARY FILES "D:/Next-python_API/next/conan_workarea/h/1c8ed01dedea1a0fcedaba3a730f4481/databridge/plugins/core/view3D_PointCloud/view3D_PointCloud.dll")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "BRIDGES_COMPONENT" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/plugins/core/view3D_PointCloud" TYPE FILE OPTIONAL FILES "D:/Next-python_API/next/conan_workarea/h/1c8ed01dedea1a0fcedaba3a730f4481/databridge/plugins/core/view3D_PointCloud/view3D_PointCloud.pdb")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "BRIDGES_COMPONENT" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/plugins/core/view3D_PointCloud" TYPE FILE FILES
    "D:/Next-python_API/next/src/bridges/bridgesdk/interface/next/bridgesdk/schema/3d_view/pointcloud.fbs"
    "D:/Next-python_API/next/conan_workarea/h/1c8ed01dedea1a0fcedaba3a730f4481/databridge/schema/3d_view/pointcloud.bfbs"
    )
endif()

