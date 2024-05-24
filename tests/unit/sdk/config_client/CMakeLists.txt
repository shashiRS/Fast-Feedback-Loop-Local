next_create_test(
  NAME config_client_test
  SRCS config_client_test.cpp
  PRIVATELINKLIBS 
    next_appsupport
    next_sdk
  PRIVATEINCLUDEDIRS 
    ${APPSUPPORT_SRC_DIR}
    ${SDK_SRC_DIR}/src/event_base
  RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/node_cfg_array.json
    ${CMAKE_CURRENT_SOURCE_DIR}/client_load_test.json
  USE_CUSTOM_ECAL_DOMAIN
)
