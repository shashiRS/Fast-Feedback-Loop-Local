next_create_test(
  NAME data_subscriber_test
  SRCS data_subscriber_test.cpp
  PRIVATELINKLIBS
    next_udex
  INSTALLTARGETS
    mts_core_lib::modules
    mts_sentinel::client
  USE_CUSTOM_ECAL_DOMAIN
)
