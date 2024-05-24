next_create_test(
  NAME signal_explorer_test
  SRCS
    signal_explorer_test.cpp
    signal_explorer_test_data.hpp
  RESOURCES
    ${CMAKE_CURRENT_LIST_DIR}/FBX4_35up_KW25_13_V5_ETHERNET1_V2_lokal_withoutETS.xml
  PRIVATELINKLIBS
    next_udex
  INSTALLTARGETS
    mts_core_lib::modules
    mts_sentinel::client
  USE_CUSTOM_ECAL_DOMAIN
  TIMEOUT 300
)
