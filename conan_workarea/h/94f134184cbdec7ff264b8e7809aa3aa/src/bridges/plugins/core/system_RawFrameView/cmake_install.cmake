# Install script for directory: D:/TATA/LATEST_NXT_REPO/next/src/bridges/plugins/core/system_RawFrameView

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "D:/TATA/LATEST_NXT_REPO/next/conan_workarea/package.next.generic.0.0.0-fallback.vs2017")
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
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/plugins/core/system_RawFrameView" TYPE SHARED_LIBRARY FILES "D:/TATA/LATEST_NXT_REPO/next/conan_workarea/h/94f134184cbdec7ff264b8e7809aa3aa/databridge/plugins/core/system_RawFrameView/system_RawFrameView.dll")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "BRIDGES_COMPONENT" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/plugins/core/system_RawFrameView" TYPE FILE OPTIONAL FILES "D:/TATA/LATEST_NXT_REPO/next/conan_workarea/h/94f134184cbdec7ff264b8e7809aa3aa/databridge/plugins/core/system_RawFrameView/system_RawFrameView.pdb")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "BRIDGES_COMPONENT" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/plugins/core/system_RawFrameView" TYPE FILE FILES
    "D:/TATA/LATEST_NXT_REPO/next/src/bridges/plugins/core/system_RawFrameView/description.json"
    "D:/TATA/LATEST_NXT_REPO/next/src/bridges/plugins/core/system_RawFrameView/raw_frame_view.fbs"
    "D:/TATA/LATEST_NXT_REPO/next/src/bridges/plugins/core/system_RawFrameView/raw_frame_plugin.ini"
    )
endif()

