next_create_test(
  NAME player_event_distributer_tests
  SRCS 
    player_event_distributer_test.cpp
  PUBLICINCLUDEDIRS
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
