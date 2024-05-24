next_create_test(
  NAME lifecycle_orchestrator_tests
  SRCS 
    lifecycle_orchestrator_test.cpp
  PRIVATEINCLUDEDIRS
    ${UNIT_TEST_COMMON_DIR}
    ${CONTROLBRIDGE_SRC_DIR}/LifeCycleOrchestrator
  PRIVATELINKLIBS
    controlbridge_lib
  INSTALLTARGETS
    mts_sentinel::client
  USE_CUSTOM_ECAL_DOMAIN
)
