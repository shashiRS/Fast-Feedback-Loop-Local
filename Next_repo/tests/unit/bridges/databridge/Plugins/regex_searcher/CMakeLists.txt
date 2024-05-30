next_create_test(
  NAME plugin_regex_searcher_test
  SRCS
    plugin_regex_searcher_test.cpp
  PRIVATEINCLUDEDIRS
    ${BRIDGESDK_INTERFACE_DIR}/next/bridgesdk/plugin_addons
    ${BRIDGESDK_SRC_DIR}/Plugin
    ${UNIT_TEST_COMMON_DIR}
    ${PLUGINS_INTERFACE_DIR}
    ${DATABRIDGE_INTERFACE_DIR}
  PRIVATELINKLIBS
    next_plugin
    Boost::boost
  Boost::regex
    next_sdk::next_appsupport
  USESDB
  INSTALLTARGETS
    mts_sentinel::client
    mts_core_lib::modules
)

