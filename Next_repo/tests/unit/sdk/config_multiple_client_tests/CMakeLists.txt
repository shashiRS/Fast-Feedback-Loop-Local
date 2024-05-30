next_create_test(
  NAME config_multiple_client_test
  SRCS config_multiple_client_test.cpp
  PRIVATELINKLIBS
    Boost::json
    next_appsupport
    next_sdk
  PRIVATEINCLUDEDIRS
    ${APPSUPPORT_SRC_DIR}
    ${SDK_SRC_DIR}/src/event_base
  RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/client_server_global_config.json
    ${CMAKE_CURRENT_SOURCE_DIR}/client_server_load_config.json
    ${CMAKE_CURRENT_SOURCE_DIR}/client_server_load_config_cmd.json
  USE_CUSTOM_ECAL_DOMAIN
)
