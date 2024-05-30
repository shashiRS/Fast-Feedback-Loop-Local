next_create_test(
  NAME event_handler_tests
  SRCS 
    meta_event_distributor_test.cpp
  PRIVATEINCLUDEDIRS
    ${CONTROLBRIDGE_SRC_DIR}/EventHandler
    ${UNIT_TEST_COMMON_DIR}
  PRIVATELINKLIBS
    Boost::boost
    controlbridge_lib
    jsoncpp_static
    next_control::next_control
  INSTALLTARGETS
     mts_sentinel::client
)
