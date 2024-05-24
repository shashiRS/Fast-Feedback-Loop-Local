find_package(Boost COMPONENTS json REQUIRED)

next_create_test(
  NAME config_base_test
  SRCS config_base_test.cpp
  PRIVATELINKLIBS
    Boost::json
    next_appsupport
    next_sdk
  PRIVATEINCLUDEDIRS 
    ${APPSUPPORT_SRC_DIR}
    ${SDK_SRC_DIR}/src/event_base
  RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/node_cfg.json
    ${CMAKE_CURRENT_SOURCE_DIR}/node_cfg_array_base.json
    ${CMAKE_CURRENT_SOURCE_DIR}/test.xml
  USE_CUSTOM_ECAL_DOMAIN
)
