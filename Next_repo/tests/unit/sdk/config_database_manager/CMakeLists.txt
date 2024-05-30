find_package(Boost COMPONENTS json REQUIRED)

next_create_test(
  NAME config_database_manager_test
  SRCS 
    config_database_manager_test.cpp
    ${APPSUPPORT_SRC_DIR}/src/config/config_database_manager.hpp
  PRIVATELINKLIBS
    next_sdk
    next_appsupport
    jsoncpp_static
  PRIVATEINCLUDEDIRS 
    ${APPSUPPORT_SRC_DIR}/src/config/
  RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/node_cfg.json
    ${CMAKE_CURRENT_SOURCE_DIR}/node_cfg_diff.json
  USE_CUSTOM_ECAL_DOMAIN
)
