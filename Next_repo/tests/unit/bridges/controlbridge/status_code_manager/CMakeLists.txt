set(TEST_PACKAGE status_code_manager_test)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS
    status_code_evaluator_test.cpp
    status_code_manager_test.cpp
  PRIVATEINCLUDEDIRS
    ${CONTROLBRIDGE_SRC_DIR}/status_code_manager
    ${UNIT_TEST_COMMON_DIR}
  PRIVATELINKLIBS
    controlbridge_lib
	next_control
    jsoncpp_static
  INSTALLTARGETS
     mts_sentinel::client
  USE_CUSTOM_ECAL_DOMAIN
)

target_compile_definitions(${TEST_PACKAGE}
  PRIVATE
    CONTROL_LIBRARY_BUILD
)
