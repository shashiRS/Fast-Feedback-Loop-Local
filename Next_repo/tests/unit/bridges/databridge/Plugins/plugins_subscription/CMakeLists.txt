set(TEST_PACKAGE plugin_subscription_test)

add_compile_definitions(BUILD_TESTING)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS
    data_subscription_test.cpp
    data_subscription_plugin.hpp
  PRIVATEINCLUDEDIRS
    ${UNIT_TEST_COMMON_DIR}
  PRIVATELINKLIBS
    databridge_lib
    Boost::date_time
    next_sdk::next_sdk
    jsoncpp_static
  INSTALLTARGETS
    mts_sentinel::client
    mts_core_lib::modules
  USE_CUSTOM_ECAL_DOMAIN
)
