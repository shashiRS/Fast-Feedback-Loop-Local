# check that Next is either used for Windows (MSVC) or Linux (gcc)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" AND WIN32)
  # nothing to do, MSVC on Windows is fine
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
  # nothing to do, gcc on Linux is fine
else()
  message(FATAL_ERROR "Not supported compiler ${CMAKE_CXX_COMPILER_ID}, only MSVC on Windows or GNU (gcc) on Linux are supported.")
endif()

# Installing the Next application and its libraries.
# The libraries are always installed at BINDIR/../lib on Linux. It is a convention on Linux to do so, and some 3rd party libraries rely on that.
# mandatory arguments:
#  COMPONENT       - name of the component, used for the install commands
# optional arguments:
#  BINDIR          - the directory, where to put the binaries (executables)
#                    if not given, CMAKE_INSTALL_BINDIR is used
#  INCDIR          - the destination directory of the include files
#                    implies INSTALLINCLUDES option
# options:
#  INSTALLINCLUDES - install the includes
#                    if INCDIR is not given CMAKE_INSTALL_INCLUDEDIR is used as target directory
#                    on Windows also the .lib files are installed into BINDIR/../lib
# examples:
#  to install the binaries into folder 'bin' and also install the includes (into the default directory):
#    next_install(BINDIR bin INSTALLINCLUDES COMPONENT MY_COMPONENT)
#  to install only the inclues into directory 'inc' (for what ever reason you would do this)
#    next_install(INCDIR inc COMPONENT MY_COMPONENT)
function(next_install)
  set(oneValueArgs BINDIR INCDIR COMPONENT)
  set(multiValueArgs "")
  set(options INSTALLINCLUDES)
  cmake_parse_arguments(NXTINST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT DEFINED NXTINST_COMPONENT)
    message(FATAL_ERROR "The name of the component for which to install Next must be given.")
  endif()

  include(GNUInstallDirs)

  if(NOT DEFINED NXTINST_BINDIR)
    message("The bin dir is not explicitly given, using ${CMAKE_INSTALL_BINDIR}")
    set(NXTINST_BINDIR ${CMAKE_INSTALL_BINDIR})
  endif()

  set(NXTINST_LIBDIR "${NXTINST_BINDIR}/../lib")

  if(${NXTINST_INSTALLINCLUDES} OR DEFINED NXTINST_INCDIR)
    if(NOT DEFINED NXTINST_INCDIR)
      set(NXTINST_INCDIR ${CMAKE_INSTALL_INCLUDEDIR})
    endif()
    install(DIRECTORY ${next_PACKAGE_PATH}/include/
      DESTINATION ${NXTINST_INCDIR}
      COMPONENT ${NXTINST_COMPONENT}
    )
    if(WIN32)
      install(DIRECTORY ${next_PACKAGE_PATH}/lib/
        DESTINATION ${NXTINST_LIBDIR}
        COMPONENT ${NXTINST_COMPONENT}
      )
    endif()
  endif()
  install(DIRECTORY ${next_PACKAGE_PATH}/bin/
    DESTINATION ${NXTINST_BINDIR}
    USE_SOURCE_PERMISSIONS
    COMPONENT ${NXTINST_COMPONENT}
  )

  # do not install the lib directory on Windows, only on Linux
  # on Windows it is installed only if the includes are also installed (see above)
  if(UNIX)
    install(DIRECTORY ${next_PACKAGE_PATH}/lib/
      DESTINATION ${NXTINST_LIBDIR}
      USE_SOURCE_PERMISSIONS
      COMPONENT ${NXTINST_COMPONENT}
    )
  endif()
endfunction()

# Installing helper scripts for starting Next out of the conan cache directory. By using this scripts, the Next
# applications do not have to be installed locally. Three scripts will be installed: start_next_controlbridge.py,
# start_next_databridge.py and start_next_player.py. As the name says each of them is starting one of the Next
# applications.
# Mandatory arguments:
#  COMPONENT - name of the component, used for the install commands
# Optional arguments:
#  BINDIR    - the directory, where to place the scripts
#              if not given, CMAKE_INSTALL_BINDIR is used
# example:
#  to install the scripts into a folder "bin", for the component "MY_COMPONENT":
#    next_install_start_scripts(BINDIR bin COMPONENT MY_COMPONENT)
function(next_install_start_scripts)
  set(oneValueArgs BINDIR COMPONENT)
  set(multiValueArgs "")
  set(options "")
  cmake_parse_arguments(NXTINST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT DEFINED NXTINST_COMPONENT)
    message(FATAL_ERROR "The name of the component for which to install the Next applications start scripts must be given.")
  endif()

  include(GNUInstallDirs)

  if(NOT DEFINED NXTINST_BINDIR)
    message("The bin dir is not explicitly given, using ${CMAKE_INSTALL_BINDIR}")
    set(NXTINST_BINDIR ${CMAKE_INSTALL_BINDIR})
  endif()

  set(NEXTCONANCACHEDIR ${next_PACKAGE_PATH})
  if(MSVC)
    set(FILEEND "bat")
  else()
    set(FILEEND "py")
  endif()
  foreach(STARTUPAPPLICATION next_controlbridge next_databridge next_player)
    configure_file(
      ${next_PACKAGE_PATH}/scripts/start_next_application.${FILEEND}.in
	  start_${STARTUPAPPLICATION}.${FILEEND}
	  USE_SOURCE_PERMISSIONS
	  @ONLY
    )
    install(PROGRAMS
      ${CMAKE_CURRENT_BINARY_DIR}/start_${STARTUPAPPLICATION}.${FILEEND}
      DESTINATION ${NXTINST_BINDIR}
      COMPONENT ${NXTINST_COMPONENT}
    )
  endforeach()
endfunction()

# Install a script for starting an application which is using the Next interface or other Next libraries without having
# the need to install the library itself. The executable will be called using the conan cache dir (the location of the
# libraries), such that the libraries are found. The function will install a script named `start_<application>` where
# the application name is a name which is given to the function as argument (see example).
# Mandatory arguments:
#  COMPONENT  - name of the component, used for the install commands
#  EXECUTABLE - name of the exectuable which shall be staretd
#               the name does not have to be a CMake target, but it has to be installed into the given BINDIR
# Optional arguments:
#  BINDIR     - the directory, wher to place the scripts
#               if not given, CMAKE_INSTALL_BINDIR is used
# Example:
#  for installing the application "my_application" for the component "MY_COMPONENT:
#    next_install_start_script(COMPONENT MY_COMPONENT EXECUTABLE my_application)
function(next_install_start_script)
  set(oneValueArgs BINDIR COMPONENT EXECUTABLE)
  set(multiValueArgs "")
  set(options "")
  cmake_parse_arguments(NXTINST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT DEFINED NXTINST_COMPONENT)
    message(FATAL_ERROR "The name of the component for which to install the Next applications start scripts must be given.")
  endif()
  if(NOT DEFINED NXTINST_EXECUTABLE)
    message(FATAL_ERROR "Name of the executable, which has to be started, is not given.")
  endif()

  include(GNUInstallDirs)

  if(NOT DEFINED NXTINST_BINDIR)
    message("The bin dir is not explicitly given, using ${CMAKE_INSTALL_BINDIR}")
    set(NXTINST_BINDIR ${CMAKE_INSTALL_BINDIR})
  endif()

  set(NEXTCONANCACHEDIR ${next_PACKAGE_PATH})
  set(STARTUPAPPLICATION ${NXTINST_EXECUTABLE})
  if(MSVC)
    set(FILEEND "bat")
  else()
    set(FILEEND "py")
  endif()
  configure_file(
    ${next_PACKAGE_PATH}/scripts/start_project_executable.${FILEEND}.in
	start_${NXTINST_EXECUTABLE}.${FILEEND}
	USE_SOURCE_PERMISSIONS
	@ONLY
  )
  install(PROGRAMS
    ${CMAKE_CURRENT_BINARY_DIR}/start_${NXTINST_EXECUTABLE}.${FILEEND}
    DESTINATION ${NXTINST_BINDIR}
    COMPONENT ${NXTINST_COMPONENT}
  )
endfunction()
