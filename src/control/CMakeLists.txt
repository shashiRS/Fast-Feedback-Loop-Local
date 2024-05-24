set(TARGET_NAME next_control)

###################################################################################
#################### add all packages to shared library ###########################
###################################################################################

set(SRC_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib_version.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/orchestrator/base_ctrl_data_generator/control_data_generator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/orchestrator/base_ctrl_data_generator/control_data_generator.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/orchestrator/orchestrator_client_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/orchestrator/orchestrator_master_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/orchestrator/orchestrator_client.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/orchestrator/orchestrator_master.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/orchestrator/orchestrator_master_impl.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/orchestrator/orchestrator_client_impl.h
  ${CONTROL_INTERFACE_DIR}/next/control/orchestrator/orchestrator_client.h
  ${CONTROL_INTERFACE_DIR}/next/control/orchestrator/orchestrator_master.h
  ${CONTROL_INTERFACE_DIR}/next/control/orchestrator/flow_control_config.h
  ${CONTROL_INTERFACE_DIR}/next/control/log_collector/log_collector.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/log_collector/log_collector.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/log_collector/log_collector_impl.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/log_collector/log_collector_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/log_collector/rotating_file_sink_log_event_data.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/log_collector/rotating_file_sink_log_event_data.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/event_control/open_recording_status_event.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/event_control/orchestrator_event_binary.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/event_control/player_state_event.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/event_control/player_command.cpp
)
set(RC_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/version_info/version_info.rc)
set(VERSION_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/version_info/version.h.in)

set(PUBLIC_LINK_LIBS
  next_sdk::next_sdk
)
set(PRIVATE_LINK_LIBS
  Boost::boost
  Boost::filesystem
  jsoncpp_static
  eCAL::core
  next_sdk::next_appsupport
  next_udex::next_udex
)

set(PUBLIC_INCLUDE_DIRS
  $<BUILD_INTERFACE:${CONTROL_INTERFACE_DIR}>
  $<INSTALL_INTERFACE:include>
)

next_create_library(
  NAME ${TARGET_NAME}
  TYPE SHARED
  SRC_FILES ${SRC_FILES}
  USE_VERSION_INFO TRUE
  FOLDER_VIEW Control
  RC_FILE ${RC_FILE}
  VERSION_FILE_PATH ${VERSION_FILE}
  TARGET_COMPILE_PRIVATE_DEF CONTROL_LIBRARY_BUILD
  TARGET_COMPILE_PUBLIC_OPT $<$<CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus>
  PUBLIC_LINK_LIBS ${PUBLIC_LINK_LIBS}
  PRIVATE_LINK_LIBS ${PRIVATE_LINK_LIBS}
  PUBLIC_INCLUDE_DIRS ${PUBLIC_INCLUDE_DIRS}
)

add_library(next_control::next_control ALIAS ${TARGET_NAME})

include(GNUInstallDirs)

install_runtime(
  TARGET_NAME ${TARGET_NAME}
  COMPONENT CONTROL_COMPONENT
  DIRECTORY ${CONTROL_INTERFACE_DIR}/
)
