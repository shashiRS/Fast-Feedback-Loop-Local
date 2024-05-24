next_create_test(
  NAME hw_data_extractor_test
  SRCS 
    hw_data_extractor_test.cpp
    ${UDEX_SRC_DIR}/src/signal_storage/core_lib_wrap_data_converter.cpp
  PRIVATELINKLIBS
    next_udex
    pocketlzma_lib
  INSTALLTARGETS
    mts_core_lib::modules
    mts_sentinel::client
  USE_CUSTOM_ECAL_DOMAIN
  RESOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/dump_hw_image_payload.lzma
)
