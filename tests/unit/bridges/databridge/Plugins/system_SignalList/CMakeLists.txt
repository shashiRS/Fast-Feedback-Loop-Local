set(TEST_PACKAGE system_SignalList_test)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS
    signal_list_test.cpp
    signal_list_test_data.hpp
    ${PLUGINS_SRC_DIR}/core/system_SignalList/signal_list.cpp
  PRIVATEINCLUDEDIRS
    ${PLUGINS_SRC_DIR}/core/system_SignalList
    ${PLUGINS_INTERFACE_DIR}
    ${PLUGIN_MANAGER_SRC_DIR}
    ${UNIT_TEST_COMMON_DIR}
    ${DATABRIDGE_INTERFACE_DIR}
  PRIVATELINKLIBS
    eCAL::core
    eCAL::pb
    flatbuffers::flatbuffers
    jsoncpp_static
    next_plugin
    next_udex
  USESDB
  INSTALLTARGETS
    mts_sentinel::client
    mts_core_lib::modules
  USE_CUSTOM_ECAL_DOMAIN
  TIMEOUT 120
)
