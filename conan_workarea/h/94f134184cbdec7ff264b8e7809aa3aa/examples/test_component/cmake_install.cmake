# Install script for directory: D:/TATA/LATEST_NXT_REPO/next/examples/test_component

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

if(CMAKE_INSTALL_COMPONENT STREQUAL "NEXT_COMPONENT" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/examples/test_component_node" TYPE EXECUTABLE FILES "D:/TATA/LATEST_NXT_REPO/next/conan_workarea/h/94f134184cbdec7ff264b8e7809aa3aa/examples/test_component/Release/test_component_node.exe")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "NEXT_COMPONENT" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/examples/test_component_node" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/TATA/LATEST_NXT_REPO/next/conan_workarea/h/94f134184cbdec7ff264b8e7809aa3aa/examples/test_component/Release/lib_test_component_node.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "NEXT_COMPONENT" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/examples/test_component_node" TYPE SHARED_LIBRARY FILES "D:/TATA/LATEST_NXT_REPO/next/conan_workarea/h/94f134184cbdec7ff264b8e7809aa3aa/examples/test_component/Release/lib_test_component_node.dll")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "NEXT_COMPONENT" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/examples/test_component_node" TYPE FILE FILES "D:/TATA/LATEST_NXT_REPO/next/examples/test_component/test_component.sdl")
endif()

