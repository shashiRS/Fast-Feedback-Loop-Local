next_create_test(
  NAME compnent_data_extractor_component
  SRCS data_extractor_test.cpp
  PRIVATEINCLUDEDIRS
    ${UDEX_SRC_DIR}/external_api/
  RESOURCES ${ECAL_INI_FILE}
  PRIVATELINKLIBS
    next_udex
  INSTALLTARGETS
    mts_core_lib::modules
    mts_sentinel::client
  USESDB
)
