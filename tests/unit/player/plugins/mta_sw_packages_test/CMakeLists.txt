next_create_test(
  NAME mta_sw_packages_test
  SRCS
    mta_sw_packages_test.cpp
  PRIVATEINCLUDEDIRS
  ${UDEX_SRC_DIR}
  ${PLAYER_SRC_DIR}
  PRIVATELINKLIBS
    next_udex
  INSTALLTARGETS
    mts_core_lib::modules
    mts_sentinel::client
  USE_CUSTOM_ECAL_DOMAIN
)
