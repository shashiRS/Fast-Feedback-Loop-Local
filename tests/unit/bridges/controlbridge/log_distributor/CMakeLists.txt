next_create_test(
  NAME log_distributor_test
  SRCS 
    log_distributor_test.cpp
    ${CONTROLBRIDGE_SRC_DIR}/log_distributor/log_distributor.cpp
  PRIVATEINCLUDEDIRS
    ${CONTROLBRIDGE_SRC_DIR}/log_distributor
    ${UNIT_TEST_COMMON_DIR}
  PRIVATELINKLIBS
    controlbridge_lib
    jsoncpp_static
  INSTALLTARGETS
     mts_sentinel::client
)
