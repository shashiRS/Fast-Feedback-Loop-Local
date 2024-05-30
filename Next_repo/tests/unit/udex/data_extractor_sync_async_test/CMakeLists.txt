next_create_test(
  NAME data_extractor_sync_async_test
  SRCS data_extractor_sync_async_test.cpp
  RESOURCES
    ${CMAKE_CURRENT_LIST_DIR}/ARS5xx.sdl
  PRIVATELINKLIBS
    next_udex
  INSTALLTARGETS
    mts_core_lib::modules
    mts_sentinel::client
  USE_CUSTOM_ECAL_DOMAIN
)
