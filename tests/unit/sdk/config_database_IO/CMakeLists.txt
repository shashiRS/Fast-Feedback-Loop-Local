find_package(Boost COMPONENTS json REQUIRED)

next_create_test(
  NAME config_database_IO_test
  SRCS 
    config_database_IO_test.cpp
    ${APPSUPPORT_SRC_DIR}/src/config/config_database_IO_impl.hpp
    ${APPSUPPORT_SRC_DIR}/src/config/config_database_IO_impl.cpp
    ${APPSUPPORT_SRC_DIR}/src/config/config_database_IO.hpp
    ${APPSUPPORT_SRC_DIR}/src/config/config_database_IO.cpp
  PRIVATELINKLIBS
    Boost::json
    Boost::filesystem
    next_sdk
    next_appsupport
    jsoncpp_static
  PRIVATEINCLUDEDIRS 
    ${APPSUPPORT_SRC_DIR}/src/config/
  RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/node_cfg.json
    ${CMAKE_CURRENT_SOURCE_DIR}/node_cfg_diff.json
    ${CMAKE_CURRENT_SOURCE_DIR}/node_cfg_invalid.json
    ${CMAKE_CURRENT_SOURCE_DIR}/test.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/test_nodes_with_attr.xml
  USE_CUSTOM_ECAL_DOMAIN
)
