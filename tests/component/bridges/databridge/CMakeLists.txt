set(TEST_PACKAGE databridge_component_test)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS 
    data_bridge_component_test.cpp
    ${BRIDGES_COMPONENT_TESTS_COMMON_INC_DIR}/websocket_client.hpp
  PRIVATEINCLUDEDIRS
    ${BRIDGESDK_INTERFACE_DIR}
    ${BRIDGES_COMPONENT_TESTS_COMMON_INC_DIR}
  PRIVATELINKLIBS
    Boost::boost
    databridge_lib
    next_sdk::next_sdk
    jsoncpp_static
  INSTALLTARGETS
    mts_sentinel::client
)
