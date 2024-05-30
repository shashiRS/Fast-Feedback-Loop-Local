set(TEST_PACKAGE log_collector_test)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS 
    log_collector_test.cpp
  PRIVATELINKLIBS
    next_control
    Boost::filesystem
  INSTALLTARGETS
    mts_sentinel::client
    mts_sentinel::sentinel
    mts_sentinel::msdia
  USE_CUSTOM_ECAL_DOMAIN
)

target_compile_definitions(${TEST_PACKAGE}
  PRIVATE
    CONTROL_LIBRARY_BUILD
)
