next_create_test(
  NAME command_handler_tests
  SRCS 
    command_parser_tests.cpp
  PRIVATEINCLUDEDIRS
    ${CONTROLBRIDGE_SRC_DIR}/CommandHandler
  PRIVATELINKLIBS
    Boost::boost
    controlbridge_lib
    next_control::next_control
  INSTALLTARGETS
    mts_sentinel::client
)
