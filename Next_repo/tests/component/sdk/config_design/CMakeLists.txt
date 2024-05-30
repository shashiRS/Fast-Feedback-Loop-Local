next_create_test(
  NAME config_design_test
  SRCS
    config_design.cpp
    eml_config_design_test.cpp
  PRIVATELINKLIBS
    next_appsupport
    next_sdk
  PRIVATEINCLUDEDIRS 
    ${APPSUPPORT_SRC_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
  RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/node_cfg.json
  USE_CUSTOM_ECAL_DOMAIN
)
