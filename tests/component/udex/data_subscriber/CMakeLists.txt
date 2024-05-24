next_create_test(
  NAME compnent_data_subscriber_component
  SRCS data_subscriber_test.cpp
  PRIVATELINKLIBS
    next_udex
  INSTALLTARGETS
    mts_core_lib::modules
    mts_sentinel::client
  USE_CUSTOM_ECAL_DOMAIN
)
