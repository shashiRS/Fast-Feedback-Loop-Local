set(TEST_PACKAGE base_ctrl_data_pool_test)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS
    base_ctrl_data_pool_test.cpp
  PRIVATELINKLIBS
    Boost::boost
    next_sdk::next_sdk
    next_sdk::next_appsupport
    next_udex::next_udex
  PRIVATEINCLUDEDIRS
    ${ORCHESTRATOR_SRC_DIR}
  RESOURCES
    ${CMAKE_CURRENT_LIST_DIR}/../resources/ARS5xx.sdl
  INSTALLTARGETS
    mts_core_lib::modules
    mts_sentinel::client
    mts_sentinel::sentinel
    mts_sentinel::msdia
  USE_CUSTOM_ECAL_DOMAIN
)

target_compile_definitions(${TEST_PACKAGE}
  PRIVATE
  CONTROL_LIBRARY_BUILD
)
