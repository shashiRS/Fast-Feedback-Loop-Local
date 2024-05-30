set(TEST_PACKAGE status_queue_manager_test)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS
    condition_test.cpp
    condition_handler_test.cpp
    status_queue_manager_test.cpp
  PRIVATEINCLUDEDIRS
    ${CONTROLBRIDGE_SRC_DIR}/status_queue_manager
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
