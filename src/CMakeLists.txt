message("###################################")
message("mts_core_lib ...        " ${mts_core_lib_PACKAGE_PATH})
message("###################################")

include(cmake/build_flatbuffers.cmake)
include(cmake/install_lib.cmake)
include(cmake/install_python_api.cmake)
include(cmake/pluginhelper.cmake)
include(cmake/testhelper.cmake)
include(cmake/version_helper.cmake)

add_subdirectory(sdk)
add_subdirectory(control)
add_subdirectory(udex)
add_subdirectory(bridges)
add_subdirectory(player)
add_subdirectory(next)

set(CONFIG_PACKAGE_LOCATION ${PROJECT_NAME}/share/cmake/${PROJECT_NAME})

# TODO split up installing the cmake files into components
install(
  FILES
    cmake/build_flatbuffers.cmake
    cmake/install_helper.cmake
    cmake/link_helper.cmake
    cmake/post_import.cmake
    cmake/pluginhelper.cmake
    cmake/testhelper.cmake
    cmake/version_helper.cmake
  DESTINATION ${CONFIG_PACKAGE_LOCATION}
  COMPONENT NEXT_COMPONENT
)

install(
  FILES
    cmake/version_info/version.h.in
    cmake/version_info/version_info.rc.in
  DESTINATION ${CONFIG_PACKAGE_LOCATION}/version_info
  COMPONENT NEXT_COMPONENT
)

if(MSVC)
  install(
    FILES
      scripts/start_next_application.bat.in
      scripts/start_project_executable.bat.in
    DESTINATION scripts
    COMPONENT NEXT_COMPONENT
  )
else()
  install(
    FILES
      scripts/start_next_application.py.in
      scripts/start_project_executable.py.in
    DESTINATION scripts
    COMPONENT NEXT_COMPONENT
  )
endif()

install_mts_sentinel(
      COMPONENT NEXT_COMPONENT
      PATH ${CMAKE_INSTALL_BINDIR}
)

install_python_api()

install(
  FILES ${ECAL_INI_FILE}
  COMPONENT NEXT_COMPONENT
  DESTINATION ${CMAKE_INSTALL_BINDIR}
)

list(APPEND LIB_LIST "eCAL::core" "eCAL::core_pb")

install_extra_libs_linux(
  LIB_LIST ${LIB_LIST}
  EXTRA_LIB_LIST mts_core_lib::modules
  COMPONENT NEXT_COMPONENT
)
