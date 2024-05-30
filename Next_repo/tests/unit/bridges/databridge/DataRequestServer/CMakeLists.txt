next_create_test(
  NAME data_request_server_test
  SRCS 
    data_request_server_tests.cpp
  PUBLICLINKLIBS
    Boost::boost
    databridge_lib
    jsoncpp_static
  INSTALLTARGETS
    mts_sentinel::client
)
