next_create_test(
  NAME compnent_data_publisher_component
  SRCS data_publisher_test.cpp
  RESOURCES
    ${CMAKE_CURRENT_LIST_DIR}/test.sdl
  PRIVATELINKLIBS
    next_udex
  INSTALLTARGETS
    mts_core_lib::modules
    mts_sentinel::client
  USE_CUSTOM_ECAL_DOMAIN
)
