# Install script for directory: D:/TATA/LATEST_NXT_REPO/next/src/control

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "D:/TATA/LATEST_NXT_REPO/next/conan_workarea/package.next.delivery.0.0.0-fallback.vs2017")
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

if(CMAKE_INSTALL_COMPONENT STREQUAL "CONTROL_COMPONENT" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/TATA/LATEST_NXT_REPO/next/conan_workarea/h/7ab8c1f1fd4e01ab32b4c38f491e59e1/src/control/Release/next_control.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "CONTROL_COMPONENT" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/TATA/LATEST_NXT_REPO/next/conan_workarea/h/7ab8c1f1fd4e01ab32b4c38f491e59e1/src/control/Release/next_control.dll")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "CONTROL_COMPONENT" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/next/share/cmake/next/CONTROL_COMPONENT/exported-targets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/next/share/cmake/next/CONTROL_COMPONENT/exported-targets.cmake"
         "D:/TATA/LATEST_NXT_REPO/next/conan_workarea/h/7ab8c1f1fd4e01ab32b4c38f491e59e1/src/control/CMakeFiles/Export/fef6c8b8ed318457663d13792ffc8288/exported-targets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/next/share/cmake/next/CONTROL_COMPONENT/exported-targets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/next/share/cmake/next/CONTROL_COMPONENT/exported-targets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/next/share/cmake/next/CONTROL_COMPONENT" TYPE FILE FILES "D:/TATA/LATEST_NXT_REPO/next/conan_workarea/h/7ab8c1f1fd4e01ab32b4c38f491e59e1/src/control/CMakeFiles/Export/fef6c8b8ed318457663d13792ffc8288/exported-targets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/next/share/cmake/next/CONTROL_COMPONENT" TYPE FILE FILES "D:/TATA/LATEST_NXT_REPO/next/conan_workarea/h/7ab8c1f1fd4e01ab32b4c38f491e59e1/src/control/CMakeFiles/Export/fef6c8b8ed318457663d13792ffc8288/exported-targets-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "CONTROL_COMPONENT" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(GET_RUNTIME_DEPENDENCIES
      RESOLVED_DEPENDENCIES_VAR _CMAKE_DEPS
      )
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "CONTROL_COMPONENT" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    foreach(_CMAKE_TMP_dep IN LISTS _CMAKE_DEPS)
      file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES ${_CMAKE_TMP_dep}
        FOLLOW_SYMLINK_CHAIN)
    endforeach()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "CONTROL_COMPONENT" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE OPTIONAL FILES "D:/TATA/LATEST_NXT_REPO/next/conan_workarea/h/7ab8c1f1fd4e01ab32b4c38f491e59e1/src/control/Release/next_control.pdb")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "CONTROL_COMPONENT" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE OPTIONAL FILES
      "D:/TATA/LATEST_NXT_REPO/next/conan_workarea/h/7ab8c1f1fd4e01ab32b4c38f491e59e1/src/udex/Release/next_udex.dll"
      "D:/TATA/LATEST_NXT_REPO/next/conan_workarea/h/7ab8c1f1fd4e01ab32b4c38f491e59e1/src/sdk/appsupport/Release/next_appsupport.dll"
      "D:/TATA/LATEST_NXT_REPO/next/conan_workarea/h/7ab8c1f1fd4e01ab32b4c38f491e59e1/src/sdk/sdk/Release/next_sdk.dll"
      "D:/.bbs_conan/0d0c747/1/bin/ecal_core_c.dll"
      "D:/.bbs_conan/0d0c747/1/bin/ecal_core.dll"
      "D:/.bbs_conan/dbe9207ff6ddb/1/bin/mts_sentinel.dll"
      "D:/.bbs_conan/7b93f5/1/bin/mtscpp.dll"
      "D:/.bbs_conan/7b93f5/1/bin/mtsc.dll"
      )
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "CONTROL_COMPONENT" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
     "D:/TATA/LATEST_NXT_REPO/next/conan_workarea/h/7ab8c1f1fd4e01ab32b4c38f491e59e1/src/control/Release/mtssentinel.exe")
    if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
      message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
      message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    file(INSTALL DESTINATION "D:/TATA/LATEST_NXT_REPO/next/conan_workarea/h/7ab8c1f1fd4e01ab32b4c38f491e59e1/src/control/Release" TYPE EXECUTABLE FILES "D:/.bbs_conan/dbe9207ff6ddb/1/bin/mtssentinel.exe")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "CONTROL_COMPONENT" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
     "D:/TATA/LATEST_NXT_REPO/next/conan_workarea/h/7ab8c1f1fd4e01ab32b4c38f491e59e1/src/control/Release/msdia140.dll")
    if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
      message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
      message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    file(INSTALL DESTINATION "D:/TATA/LATEST_NXT_REPO/next/conan_workarea/h/7ab8c1f1fd4e01ab32b4c38f491e59e1/src/control/Release" TYPE SHARED_LIBRARY FILES "D:/.bbs_conan/dbe9207ff6ddb/1/bin/msdia140.dll")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "CONTROL_COMPONENT" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES "D:/TATA/LATEST_NXT_REPO/next/src/control/interface/")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "CONTROL_COMPONENT" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/.bbs_conan/dbe9207ff6ddb/1/bin/mtssentinel.exe")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "CONTROL_COMPONENT" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/.bbs_conan/dbe9207ff6ddb/1/bin/msdia140.dll")
  endif()
endif()

