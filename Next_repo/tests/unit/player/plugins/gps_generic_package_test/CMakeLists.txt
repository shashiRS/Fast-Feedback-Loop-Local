next_create_test(
  NAME gps_generic_package_test
  SRCS
  gps_generic_package_test.cpp
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
