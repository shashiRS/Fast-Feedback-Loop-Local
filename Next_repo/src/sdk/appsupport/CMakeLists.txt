set(TARGET_NAME next_appsupport)

set(SRC_FILES
  ${APPSUPPORT_INTERFACE_DIR}/next/appsupport/config/config_types.hpp
  src/config/config_base.cpp
  ${APPSUPPORT_INTERFACE_DIR}/next/appsupport/config/config_base.hpp
  src/config/config_client.cpp
  ${APPSUPPORT_INTERFACE_DIR}/next/appsupport/config/config_client.hpp
  src/config/config_base_impl.cpp
  src/config/config_base_impl.hpp
  src/config/config_change_hook_handler.cpp
  src/config/config_change_hook_handler.hpp
  src/config/config_comp_data.cpp
  src/config/config_comp_data.hpp
  src/config/config_comp_data_impl.cpp
  src/config/config_comp_data_impl.hpp
  src/config/config_comp_data_storage.hpp
  src/config/config_data_datatypes.hpp
  src/config/config_database_IO.cpp
  src/config/config_database_IO.hpp
  src/config/config_database_IO_impl.cpp
  src/config/config_database_IO_impl.hpp
  src/config/config_database_manager.cpp
  src/config/config_database_manager.hpp
  src/config/config_database_manager_impl.cpp
  src/config/config_database_manager_impl.hpp
  src/config/config_utilities.hpp
  src/config/config_server.cpp
  ${APPSUPPORT_INTERFACE_DIR}/next/appsupport/config/config_server.hpp
  src/cmd_options.cpp
  ${APPSUPPORT_INTERFACE_DIR}/next/appsupport/cmd_options/cmd_options.hpp
  src/cmd_options_impl.cpp
  src/cmd_options_impl.hpp
  src/license_checker/license_checker.cpp
  src/license_checker/license_checker.hpp
  src/life_cycle/lifecycle_base.cpp
  ${APPSUPPORT_INTERFACE_DIR}/next/appsupport/log_connector/log_connector.hpp
  src/log_connector/log_connector.cpp
  ${APPSUPPORT_INTERFACE_DIR}/next/appsupport/next_status_code/next_status_code.hpp
  src/next_status_code/next_status_code.cpp
  ${APPSUPPORT_INTERFACE_DIR}/next/appsupport/lifecycle/lifecycle_base.hpp
  src/config/config_change_hook_collection.cpp
  ${APPSUPPORT_INTERFACE_DIR}/next/appsupport/config/config_change_hook_collection.hpp
  ${APPSUPPORT_INTERFACE_DIR}/next/appsupport/config/config_key_generator.hpp
  ${APPSUPPORT_INTERFACE_DIR}/next/appsupport/config/config_key_generator_orchestrator.hpp
  ${APPSUPPORT_INTERFACE_DIR}/next/appsupport/config/config_key_generator_player.hpp
  ${APPSUPPORT_INTERFACE_DIR}/next/appsupport/config/config_key_generator_databridge.hpp
  ${APPSUPPORT_INTERFACE_DIR}/next/appsupport/config/config_key_generator_logger.hpp
  ${APPSUPPORT_INTERFACE_DIR}/next/appsupport/session_handler/session_server.hpp
  ${APPSUPPORT_INTERFACE_DIR}/next/appsupport/session_handler/session_client.hpp
  src/session_handler/session_server.cpp
  src/session_handler/session_server_impl.cpp
  src/session_handler/session_server_impl.hpp
  src/session_handler/session_client.cpp
  src/session_handler/session_client_impl.cpp
  src/session_handler/session_client_impl.hpp
  src/startup.cpp
  src/system_information.h
  ${APPSUPPORT_INTERFACE_DIR}/next/appsupport/config/config_tags.hpp
)

source_group("Config Header Files" FILES 
  ${APPSUPPORT_INTERFACE_DIR}/next/appsupport/config/config_types.hpp
  ${APPSUPPORT_INTERFACE_DIR}/next/appsupport/config/config_base.hpp
  ${APPSUPPORT_INTERFACE_DIR}/next/appsupport/config/config_client.hpp
  src/config/config_base_impl.hpp
  src/config/config_change_hook_handler.hpp
  src/config/config_comp_data.hpp
  src/config/config_comp_data_impl.hpp
  src/config/config_comp_data_storage.hpp
  src/config/config_data_datatypes.hpp
  src/config/config_database_IO.hpp
  src/config/config_database_IO_impl.hpp
  src/config/config_database_manager.hpp
  src/config/config_database_manager_impl.hpp
  src/config/config_utilities.hpp
  ${APPSUPPORT_INTERFACE_DIR}/next/appsupport/config/config_server.hpp
  ${APPSUPPORT_INTERFACE_DIR}/next/appsupport/config/config_change_hook_collection.hpp
  ${APPSUPPORT_INTERFACE_DIR}/next/appsupport/config/config_key_generator.hpp
  ${APPSUPPORT_INTERFACE_DIR}/next/appsupport/config/config_key_generator_orchestrator.hpp
  ${APPSUPPORT_INTERFACE_DIR}/next/appsupport/config/config_key_generator_player.hpp
  ${APPSUPPORT_INTERFACE_DIR}/next/appsupport/config/config_key_generator_logger.hpp
  ${APPSUPPORT_INTERFACE_DIR}/next/appsupport/config/config_tags.hpp
)

source_group("Config Source Files" FILES 
  src/config/config_base.cpp
  src/config/config_base_impl.cpp
  src/config/config_client.cpp
  src/config/config_change_hook_handler.cpp
  src/config/config_change_hook_collection.cpp
  src/config/config_comp_data.cpp
  src/config/config_comp_data_impl.cpp
  src/config/config_database_IO.cpp
  src/config/config_database_IO_impl.cpp
  src/config/config_server.cpp 
  src/config/config_database_manager.cpp
  src/config/config_database_manager_impl.cpp
)

set(PUBLIC_LINK_LIBS next_sdk)
set(PRIVATE_LINK_LIBS
  Boost::boost
  Boost::system
  Boost::filesystem
  Boost::program_options
  jsoncpp_static
)

set(PUBLIC_INCLUDE_DIRS
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${APPSUPPORT_INTERFACE_DIR}>
)
set(PRIVATE_INCLUDE_DIRS
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
)

next_create_library(
  NAME ${TARGET_NAME}
  TYPE SHARED
  SRC_FILES ${SRC_FILES}
  USE_VERSION_INFO TRUE
  FOLDER_VIEW SDK
  TARGET_COMPILE_PRIVATE_DEF _CRT_SECURE_NO_WARNINGS
  TARGET_COMPILE_PUBLIC_OPT $<$<CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus>
  PUBLIC_LINK_LIBS ${PUBLIC_LINK_LIBS}
  PRIVATE_LINK_LIBS ${PRIVATE_LINK_LIBS}
  PUBLIC_INCLUDE_DIRS ${PUBLIC_INCLUDE_DIRS}
  PRIVATE_INCLUDE_DIRS ${PRIVATE_INCLUDE_DIRS}
)

configure_file(config.h.in include/config.h)

########################### Install
include(GNUInstallDirs)

add_library(next_sdk::next_appsupport ALIAS ${TARGET_NAME})

install_runtime(
  TARGET_NAME ${TARGET_NAME}
  COMPONENT SDK_COMPONENT
  DIRECTORY ${APPSUPPORT_INTERFACE_DIR}/
)
