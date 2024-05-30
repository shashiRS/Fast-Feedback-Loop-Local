next_create_test(
  NAME data_broadcaster_test
  SRCS data_broadcaster_test.cpp
  PRIVATELINKLIBS
    databridge_lib
    Boost::date_time
  INSTALLTARGETS
    mts_sentinel::client
)
