next_create_test(
  NAME request_event_handler_test
  SRCS
  request_event_handler_test.cpp
  PRIVATELINKLIBS
    Boost::boost
    databridge_lib
    jsoncpp_static
  INSTALLTARGETS
    mts_sentinel::client
  USESDB
)
