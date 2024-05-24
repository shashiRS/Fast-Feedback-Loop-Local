# Install script for directory: D:/TATA/LATEST_NXT_REPO/next

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

if(CMAKE_INSTALL_COMPONENT STREQUAL "PACKAGE_INFO" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/next/share/cmake/next" TYPE FILE FILES "D:/TATA/LATEST_NXT_REPO/next/conan_workarea/h/94f134184cbdec7ff264b8e7809aa3aa/next-config.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "PACKAGE_INFO" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/next/share/cmake/next" TYPE FILE FILES "D:/TATA/LATEST_NXT_REPO/next/conan_workarea/h/94f134184cbdec7ff264b8e7809aa3aa/next-config-version.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "PACKAGE_INFO" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/.bricks" TYPE FILE OPTIONAL FILES "D:/TATA/LATEST_NXT_REPO/next/conan_workarea/h/94f134184cbdec7ff264b8e7809aa3aa/compile_commands.json")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/TATA/LATEST_NXT_REPO/next/conan_workarea/h/94f134184cbdec7ff264b8e7809aa3aa/src/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/TATA/LATEST_NXT_REPO/next/conan_workarea/h/94f134184cbdec7ff264b8e7809aa3aa/examples/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "__BRICKS_IMPLICIT_DEPENDENCIES" OR NOT CMAKE_INSTALL_COMPONENT)
  include("D:/TATA/LATEST_NXT_REPO/next/conan_workarea/h/94f134184cbdec7ff264b8e7809aa3aa/.bricks/d/install_dependencies.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "D:/TATA/LATEST_NXT_REPO/next/conan_workarea/h/94f134184cbdec7ff264b8e7809aa3aa/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
