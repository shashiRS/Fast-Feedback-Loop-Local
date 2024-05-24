# Install script for directory: D:/TATA/LATEST_NXT_REPO/next/src

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

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/TATA/LATEST_NXT_REPO/next/conan_workarea/h/94f134184cbdec7ff264b8e7809aa3aa/src/sdk/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/TATA/LATEST_NXT_REPO/next/conan_workarea/h/94f134184cbdec7ff264b8e7809aa3aa/src/control/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/TATA/LATEST_NXT_REPO/next/conan_workarea/h/94f134184cbdec7ff264b8e7809aa3aa/src/udex/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/TATA/LATEST_NXT_REPO/next/conan_workarea/h/94f134184cbdec7ff264b8e7809aa3aa/src/bridges/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/TATA/LATEST_NXT_REPO/next/conan_workarea/h/94f134184cbdec7ff264b8e7809aa3aa/src/player/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/TATA/LATEST_NXT_REPO/next/conan_workarea/h/94f134184cbdec7ff264b8e7809aa3aa/src/next/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "NEXT_COMPONENT" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/next/share/cmake/next" TYPE FILE FILES
    "D:/TATA/LATEST_NXT_REPO/next/src/cmake/build_flatbuffers.cmake"
    "D:/TATA/LATEST_NXT_REPO/next/src/cmake/install_helper.cmake"
    "D:/TATA/LATEST_NXT_REPO/next/src/cmake/link_helper.cmake"
    "D:/TATA/LATEST_NXT_REPO/next/src/cmake/post_import.cmake"
    "D:/TATA/LATEST_NXT_REPO/next/src/cmake/pluginhelper.cmake"
    "D:/TATA/LATEST_NXT_REPO/next/src/cmake/testhelper.cmake"
    "D:/TATA/LATEST_NXT_REPO/next/src/cmake/version_helper.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "NEXT_COMPONENT" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/next/share/cmake/next/version_info" TYPE FILE FILES
    "D:/TATA/LATEST_NXT_REPO/next/src/cmake/version_info/version.h.in"
    "D:/TATA/LATEST_NXT_REPO/next/src/cmake/version_info/version_info.rc.in"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "NEXT_COMPONENT" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/scripts" TYPE FILE FILES
    "D:/TATA/LATEST_NXT_REPO/next/src/scripts/start_next_application.bat.in"
    "D:/TATA/LATEST_NXT_REPO/next/src/scripts/start_project_executable.bat.in"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "NEXT_COMPONENT" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/.bbs_conan/dbe9207ff6ddb/1/bin/mtssentinel.exe")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "NEXT_COMPONENT" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/.bbs_conan/dbe9207ff6ddb/1/bin/msdia140.dll")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "NEXT_COMPONENT" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/next_python" TYPE DIRECTORY FILES "D:/TATA/LATEST_NXT_REPO/next/src/py_api/" FILES_MATCHING REGEX "/[^/]*$" REGEX "/\\_\\_pycache\\_\\_$" EXCLUDE)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "NEXT_COMPONENT" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "D:/TATA/LATEST_NXT_REPO/next/src/conf/ecal.ini")
endif()

