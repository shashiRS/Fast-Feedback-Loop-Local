set(TEST_PACKAGE orchestrator_test)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS
    enhanced_test_orchestrator.cpp
  PRIVATELINKLIBS
    next_control
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
