set(TARGET_NAME next_player)

set(SRC_FILES
  src/play_manager.hpp
  src/play_manager.cpp
  src/main.cpp
  src/state_machine/player_state_machine.hpp
  src/state_machine/player_state_machine.cpp
  src/state_machine/error_codes.hpp
  src/state_machine/states/state_error.hpp
  src/state_machine/states/state_machine_events.hpp
  src/state_machine/states/state_opening.cpp
  src/state_machine/states/state_opening.hpp
  src/state_machine/states/state_play.cpp
  src/state_machine/states/state_play.hpp
  src/state_machine/states/state_ready.hpp
  src/state_machine/states/state_shutdown.hpp
  src/state_machine/states/state_skip_to.hpp
  src/state_machine/states/state_unload_recording.hpp
  src/sdk_deprecated/threadsafe_queue.hpp
  src/resource_manager/buffer_queue.hpp
  src/resource_manager/cycle_manager.cpp
  src/resource_manager/cycle_manager.hpp
  src/resource_manager/resource_manager.cpp
  src/resource_manager/resource_manager.hpp
  src/resource_manager/resource_manager_interface.hpp
  src/resource_manager/udex_service.cpp
  src/resource_manager/udex_service.hpp
  src/reader/file_reader.cpp
  src/reader/file_reader.hpp
  src/reader/reader_manager.cpp
  src/reader/reader_manager.hpp
  src/reader/reader_synchronizer.cpp
  src/reader/reader_synchronizer.hpp
  src/publisher/publisher_manager.cpp
  src/publisher/publisher_manager.hpp
  src/publisher/speed_factor.cpp
  src/publisher/speed_factor.hpp
  src/publisher/trigger_handler.hpp
  src/publisher/trigger_mapper.cpp
  src/publisher/trigger_mapper.hpp
  src/plugin/core_lib_reader/core_lib_reader_impl.hpp
  src/plugin/core_lib_reader/core_lib_reader.h
  src/plugin/core_lib_reader/core_lib_reader.cpp
  src/plugin/core_lib_reader/core_lib_package.h
  src/plugin/core_lib_reader/core_lib_datatypes.h
  src/plugin/core_lib_reader/package_helpers/can_package_helper.h
  src/plugin/core_lib_reader/package_helpers/default_package_helper.h
  src/plugin/core_lib_reader/package_helpers/ethernet_package_helper.h
  src/plugin/core_lib_reader/package_helpers/flexray_package_helper.h
  src/plugin/core_lib_reader/package_helpers/gps_package_helper.h
  src/plugin/core_lib_reader/package_helpers/mtahw_package_helper.h
  src/plugin/core_lib_reader/package_helpers/mtasw_package_helper.h
  src/plugin/core_lib_reader/package_helpers/rtrange_package_helper.h
)

set(RC_FILE ${CMAKE_CURRENT_SOURCE_DIR}/version_info/version_info.rc)
set(VERSION_FILE ${CMAKE_CURRENT_SOURCE_DIR}/version_info/version.h.in)

set(PRIVATE_LINK_LIBS
  Boost::system
  Boost::filesystem
  Boost::date_time # required by filesystem
  eCAL::core
  jsoncpp_static
  mtscpp::mtscpp # Magno SDK link dependency
  mts_ecu::mts_ecu # Magno ECU Toolbox, CMake interface library
  mts_can::mts_can # Magno CAN Toolbox, CMake interface library
  mts_eth::mts_eth # Magno Ethernet Toolbox, CMake interface library
  mts_flexray::mts_flexray # Magno FlexRay Toolbox, CMake interface library
  mts_rt_range::mts_rt_range # Magno Reference Sensor Toolbox, CMake interface library
  mts_gps::mts_gps # Magno Reference Sensor Toolbox, CMake interface library
  mts_ref_cam::mts_ref_cam # Magno Reference Sensor Toolbox, CMake interface library
  next_control::next_control
  next_sdk::next_sdk
  next_sdk::next_appsupport
  next_udex::next_udex
)

set(PUBLIC_INCLUDE_DIRS
  $<BUILD_INTERFACE:${PLAYER_INTERFACE_DIR}>
  $<INSTALL_INTERFACE:include>
)

next_create_executable(
  NAME ${TARGET_NAME}
  SRC_FILES ${SRC_FILES}
  USE_VERSION_INFO TRUE
  RC_FILE ${RC_FILE}
  VERSION_FILE_PATH ${VERSION_FILE}
  FOLDER_VIEW Player
  PRIVATE_LINK_LIBS ${PRIVATE_LINK_LIBS}
  PUBLIC_INCLUDE_DIRS ${PUBLIC_INCLUDE_DIRS}
)

########################### Install
include(GNUInstallDirs)

install_runtime(
  TARGET_NAME ${TARGET_NAME}
  COMPONENT PLAYER_COMPONENT
  FILES ${CMAKE_SOURCE_DIR}/src/player/conf/playerdefaultconfig.json
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/interface/
)

list(APPEND LIB_LIST "eCAL::core" "eCAL::core_pb")

install_extra_libs_linux(
  LIB_LIST ${LIB_LIST}
  EXTRA_LIB_LIST mts_core_lib::modules
  COMPONENT PLAYER_COMPONENT
)

install_extra_libs_windows(
  TARGET_NAME ${TARGET_NAME}
  LIB_LIST mts_core_lib::modules
  COMPONENT PLAYER_COMPONENT
)

# the variable CMAKE_IMPORT_LIBRARY_SUFFIX is non empty on DLL platforms
if(CMAKE_IMPORT_LIBRARY_SUFFIX)

  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/player/conf/playerdefaultconfig.json
      ${CMAKE_SOURCE_DIR}/src/player/conf/trigger_config.json
      ${ECAL_INI_FILE} $<TARGET_FILE_DIR:${TARGET_NAME}>/..
  )
else()

  if(NOT "$ENV{CI}" STREQUAL "true")
    # copy the needed configuration files into build folder (for debugging purposes)

    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/player/conf/playerdefaultconfig.json
        ${CMAKE_SOURCE_DIR}/src/player/conf/trigger_config.json
        ${ECAL_INI_FILE} $<TARGET_FILE_DIR:${TARGET_NAME}>
    )
  endif()
endif(CMAKE_IMPORT_LIBRARY_SUFFIX)
