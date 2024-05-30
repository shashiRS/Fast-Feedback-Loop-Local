next_create_test(
  NAME udex_data_publisher_test
  SRCS data_publisher_test.cpp
  RESOURCES
  ${CMAKE_CURRENT_LIST_DIR}/../resources/arrays.sdl
  PRIVATELINKLIBS
    next_udex
  INSTALLTARGETS
    mts_core_lib::modules
    mts_sentinel::client
  USE_CUSTOM_ECAL_DOMAIN
)
